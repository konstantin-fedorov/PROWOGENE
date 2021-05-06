#include "river.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "utils/array2d_tools.h"
#include "utils/range.h"

namespace prowogene {
namespace modules {

using std::list;
using std::string;
using std::vector;
using utils::Array2D;
using utils::Random;
using utils::Range;
using AT = utils::Array2DTools;

static const float kDistortionScale = sqrt(2.0f) / 4.0f;

void RiverModule::Init() {
    const int chunk_size = settings_.general.chunk_size;
    const int chunks_count = settings_.general.size / chunk_size;

    rand_ = new Random(settings_.general.seed);
    min_.Resize(chunks_count, chunks_count);
    max_.Resize(chunks_count, chunks_count);
}

void RiverModule::Deinit() {
    if (rand_) {
        delete rand_;
        rand_ = nullptr;
    }
    min_.Clear();
    max_.Clear();
    last_points_.clear();
}

bool RiverModule::Process() {
    if (!settings_.river.count) {
        return true;
    }

    const int size = settings_.general.size;
    if (river_mask_->Width() != size ||
            river_mask_->Height() != size) {
        river_mask_->Resize(size, size, 0);
    }

    ScanChunks();
    DetecLastPoints();
    if (!last_points_.size()) {
        return true;
    }

    Array2D<uint8_t> river_mask(size, size, 0);
    for (int i = 0; i < settings_.river.count; ++i) {
        CreateRiver(settings_.river.settings[i], river_mask);
    }

    const int radius = settings_.river.smooth_radius;
    const int threads = settings_.system.thread_count;
    AT::Smooth(*height_map_, std::max(2, radius), threads);
    AT::ToRange(*height_map_, 0.0f, 1.0f, threads);

    return true;
}

list<string> RiverModule::GetNeededSettings() const {
    return {
        kConfigGeneral,
        kConfigRiver,
        kConfigSystem
    };
}

void RiverModule::ApplySettings(ISettings *settings) {
    CopySettings(settings, settings_.general);
    CopySettings(settings, settings_.river);
    CopySettings(settings, settings_.system);
}

std::string RiverModule::GetName() const {
    return "River";
}

void RiverModule::ScanChunks() {
    const int chunk_size = settings_.general.chunk_size;
    const int chunks_count = settings_.general.size / chunk_size;

    const int chunck_size = height_map_->Width() / chunks_count;
    for (int x = 0; x < chunks_count; ++x) {
        for (int y = 0; y < chunks_count; ++y) {
            const int start_x = x * chunck_size;
            const int start_y = y * chunck_size;
            const float start_val = (*height_map_)(start_x, start_y);
            auto& min = min_(x, y);
            auto& max = max_(x, y);
            min.value = start_val;
            min.x = start_x;
            min.y = start_y;

            max.value = start_val;
            max.x = start_x;
            max.y = start_y;
            AT::FindMinMaxInArea(min, max, *height_map_, start_x, start_y,
                                 chunck_size, chunck_size);
        }
    }
}

void RiverModule::DetecLastPoints() {
    const int chunk_size = settings_.general.chunk_size;
    const int chunks_count = settings_.general.size / chunk_size;
    for (int x = 0; x < chunks_count; ++x) {
        for (int y = 0; y < chunks_count; ++y) {
            Point<float> p = min_(x, y);
            if (p.value < *sea_level_) {
                last_points_.push_back(p);
            }
        }
    }
}

void RiverModule::CreateRiver(const SingleRiverSettings& river,
                              Array2D<uint8_t> river_mask) {
    const int max_point_idx = static_cast<int>(last_points_.size()) - 1;
    const int bottom_point_idx = rand_->Next(0, max_point_idx);
    const Point<float> bottom_point = last_points_[bottom_point_idx];
    Point<float> top_point = bottom_point;

    const int length = river.max_length_in_chunks;
    const int chunk_size = settings_.general.chunk_size;
    const int chunks_count = settings_.general.size / chunk_size;
    const int chunk_x = bottom_point.x / chunk_size;
    const int chunk_y = bottom_point.y / chunk_size;

    Range ch_r(std::max(0,            chunk_x - length),
               std::min(chunks_count, chunk_x + length),
               std::max(0,            chunk_y - length),
               std::min(chunks_count, chunk_y + length));

    for (int x = ch_r.left; x < ch_r.right; ++x) {
        for (int y = ch_r.top; y < ch_r.bottom; y++) {
            const auto& cur_point = max_(x, y);
            if (cur_point.value > top_point.value &&
                cur_point.value < river.max_head_height) {
                top_point = cur_point;
            }
        }
    }

    auto palette = CreatePalette(river.channel.width);
    MarkChannel(top_point, bottom_point, river, river_mask);
    CreateChannelLadder(river_mask, river.channel.width);
    BumpChannel(river_mask, river, bottom_point.value, palette);
    PostBumpChannel(top_point.x, top_point.y, river_mask,
                   (*height_map_)(top_point.x, top_point.y),
                   river.channel.width, bottom_point.value, palette);
}

void RiverModule::MarkChannel(const Point<float>& top,
        const Point<float>& bottom, const SingleRiverSettings& river,
        Array2D<uint8_t>& mask) const {
    const int size = settings_.general.size;
    const int dx = std::abs(top.x - bottom.x);
    const int dy = std::abs(top.y - bottom.y);

    if (dx > 1 || dy > 1) {
        Point<float> mid = ChooseMiddlePoint(top, bottom, river.distortion);
        mask(mid.x, mid.y) = 1;
        MarkChannel(top, mid, river, mask);
        MarkChannel(mid, bottom, river, mask);
    }
}

Point<float> RiverModule::ChooseMiddlePoint(const Point<float>& top,
                                            const Point<float>& bottom,
                                            float distortion) const {
    const int size = settings_.general.size;
    const int dx = std::abs(top.x - bottom.x);
    const int dy = std::abs(top.y - bottom.y);
    const int x = (top.x + bottom.x) / 2;
    const int y = (top.y + bottom.y) / 2;
    const float real_distortion = kDistortionScale * distortion;
    float min = (*height_map_)(x, y);
    Point<float> middle_point;
    middle_point.x = x;
    middle_point.y = y;

    if (dx >= dy) {
        const float shift = std::round(-0.5f + real_distortion * dx);
        const int radius = static_cast<int>(shift);
        const int top = std::max(0, y - radius);
        const int bottom = std::min(size, y + radius);
        for (int i = top; i < bottom; ++i) {
            if ((*height_map_)(x, i) < min) {
                min = (*height_map_)(x, i);
                middle_point.y = i;
            }
        }
    } else {
        const float shift = std::round(-0.5f + real_distortion * dy);
        const int radius = static_cast<int>(shift);
        int left = std::max(0, x - radius);
        int right = std::min(size, x + radius);
        for (int i = left; i < right; ++i) {
            if ((*height_map_)(i, y) < min) {
                min = (*height_map_)(i, y);
                middle_point.x = i;
            }
        }
    }
    return middle_point;
}

void RiverModule::CreateChannelLadder(Array2D<uint8_t>& river_mask,
                                      int river_width) const {
    const int size = settings_.general.size;

    for (int w = 2; w < river_width; ++w) {
        for (int x = 0; x < size; ++x) {
            for (int y = 0; y < size; ++y) {
                if (river_mask(x, y) == w - 1) {
                    Range mask_r(std::max(0,    x - 1),
                                 std::min(size, x + 1),
                                 std::max(0,    y - 1),
                                 std::min(size, y + 1));
                    for (int k = mask_r.left; k <= mask_r.right; ++k) {
                        for (int l = mask_r.top; l <= mask_r.bottom; ++l) {
                            if (!river_mask(k, l)) {
                                river_mask(k, l) = w;
                            }
                        }
                    }
                }
            }
        }
    }
}

void RiverModule::BumpChannel(Array2D<uint8_t>& mask,
        const SingleRiverSettings& river, float bottom,
        const vector<float>& palette) const {
    const int channel_width = river.channel.width;
    const int size = settings_.general.size;

    const float max_bump = palette.back() * river.channel.depth;

    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            if (mask(x, y)) {
                auto& height_el = (*height_map_)(x, y);
                auto& river_el = (*river_mask_)(x, y);
                const float palette_val = palette[mask(x, y) - 1];
                float bump = palette_val * river.channel.depth;
                if (height_el - bump < bottom) {
                    bump = height_el - bottom;
                }
                height_el = std::max(0.0f, height_el - bump);
                river_el = std::max(river_el, palette_val * 255.0f);
            }
        }
    }
}

void RiverModule::PostBumpChannel(int x, int y, Array2D<uint8_t>& mask,
        float prev_height, int channel_radius, float bottom,
        const std::vector<float>& palette) const {
    const int size = settings_.general.size;
    float& center = (*height_map_)(x, y);

    if (center > prev_height) {
        Range range(std::max(0,    x - channel_radius),
                    std::min(size, x + channel_radius),
                    std::max(0,    y - channel_radius),
                    std::min(size, y + channel_radius));
        float delta = center - prev_height;
        if (center - delta < bottom) {
            delta = center - bottom;
        }

        for (int k = range.left; k <= range.right; ++k) {
            for (int l = range.top; l <= range.bottom; ++l) {
                float& height_el = (*height_map_)(k, l);
                if (mask(k, l) > 1) {
                    height_el -= delta * palette[mask(k, l)];
                    mask(k, l) = 0;
                }
            }
        }
        center -= delta;
    }

    mask(x, y) = 0;
    if (x > 0 && y > 0 && x < size - 1 && y < size - 1) {
        float min = 1.0f;
    }
    Range range(std::max(0, x - 1), std::min(size, x + 1),
                std::max(0, y - 1), std::min(size, y + 1));
    for (int k = range.left; k <= range.right; ++k) {
        for (int l = range.top; l <= range.bottom; ++l) {
            if (mask(k, l) == 1) {
                PostBumpChannel(k, l, mask, center, channel_radius,
                    bottom, palette);
            }
        }
    }
}

vector<float> RiverModule::CreatePalette(int width) {
    vector<float> palette(width, 0.0f);
    const float coef = static_cast<float>(M_PI) /
                       static_cast<float>(width);
    for (int i = 0; i < width; ++i) {
        palette[i] = std::cos(coef * static_cast<float>(i));
        palette[i] += 1.0f;
        palette[i] /= 2.0f;
    }
    return palette;
}

} // namescpace modules
} // namescpace prowogene
