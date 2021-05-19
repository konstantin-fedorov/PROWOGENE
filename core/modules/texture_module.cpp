#include "texture.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <functional>
#include <thread>
#include <tuple>
#include <utility>

#include "utils/array2d.h"
#include "utils/array2d_tools.h"
#include "utils/types_converter.h"
#include "utils/range.h"


namespace prowogene {
namespace modules {

using std::list;
using std::pair;
using std::string;
using std::thread;
using std::tuple;
using std::vector;
using utils::Array2D;
using utils::Image;
using utils::ImageIO;
using utils::ImageIOParams;
using utils::Random;
using utils::Range;
using utils::RgbaPixel;
using AT = utils::Array2DTools;
using TC = utils::TypesConverter;

static const int kAlphaBandAccuracy = 4096;

void TextureModule::Process() {
    const int size = settings_.general.size;
    const int chunk_size = settings_.general.chunk_size;
    const int chunks_count = size / chunk_size;

    if (settings_.texture.height_map_enabled) {
        SaveHeightMap();
    }

    if (!settings_.texture.chunks_enabled &&
            !settings_.texture.minimap.enabled) {
        return;
    }
    try {
        ReadReferenceTextures();
    }
    catch (const LogicException& e) {
        if (!settings_.texture.gradient.enabled ||
                settings_.texture.gradient.opacity < 1.0f - kEps) {
            throw LogicException(std::string(e.what()) +
                " Also gradient is not enabled or it's opacity is not 1.0." +
                "Can't fill texture with colors.");
        }
    }

    InitAlphaBands();
    if (settings_.texture.gradient.enabled) {
        InitGradient();
    }
    if (settings_.texture.minimap.enabled) {
        int minimap_size = settings_.texture.minimap.tile_size;
        minimap_size *= chunks_count;
        minimap_.Resize(minimap_size, minimap_size);
    }

    int treads_count = settings_.system.thread_count;
    if (treads_count < 1) {
        treads_count = thread::hardware_concurrency();
        treads_count = treads_count > 1 ? treads_count : 1;
    }
    int block_size = chunks_count / treads_count;
    if (!block_size) {
        treads_count = chunks_count;
        block_size = 1;
    }

    vector<thread> threads(treads_count);
    for (int i = 0; i < treads_count; ++i) {
        const int beg = i * block_size;
        int end = (i + 1) * block_size;
        if (i == threads.size() - 1) {
            end = chunks_count;
        }
        threads[i] = thread(&TextureModule::ProcessTiles, this, beg, end);
    }
    for (int i = 0; i < treads_count; ++i) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }

    if (settings_.texture.minimap.enabled) {
        ImageIOParams params;
        params.filename = settings_.names.minimap.texture;
        params.bit_depth = settings_.texture.target_bitdepth;
        params.format = settings_.system.extensions.image;
        params.quality = 0;
        image_io_->Save(minimap_, params);
        if (settings_.texture.normals.enabled) {
            Image normal;
            CreateNormal(minimap_, settings_.texture.normals.invert,
                         settings_.texture.normals.strength, normal);
            params.filename = settings_.names.minimap.normal;
            image_io_->Save(normal, params);
        }
    }
}

void TextureModule::Deinit() {
    grad_table_.clear();
    alpha_bands_.clear();
    reference_textures_.clear();
    reference_textures_heights_.clear();
    reference_decals_.clear();
    minimap_.Clear();
}

list<string> TextureModule::GetNeededSettings() const {
    return {
        settings_.basis.GetName(),
        settings_.general.GetName(),
        settings_.names.GetName(),
        settings_.system.GetName(),
        settings_.texture.GetName()
    };
}

void TextureModule::ApplySettings(ISettings* settings) {
    CopySettings(settings, settings_.basis);
    CopySettings(settings, settings_.general);
    CopySettings(settings, settings_.names);
    CopySettings(settings, settings_.system);
    CopySettings(settings, settings_.texture);
}

std::string TextureModule::GetName() const {
    return "Texture";
}

void TextureModule::ReadReferenceTextures() {
    const int tile_size = settings_.texture.minimap.tile_size;
    const auto& img_bases = settings_.texture.images.bases;
    const auto& img_decals = settings_.texture.images.decals;

    const int biomes_count = TC::ToInt(Biome::Count);
    reference_textures_.resize(biomes_count);
    reference_decals_.resize(biomes_count);
    reference_textures_heights_.resize(biomes_count);

    const vector<tuple<Biome, string, vector<string> > > info = {
        { Biome::Basis,    img_bases.basis,    img_decals.basis },
        { Biome::Mountain, img_bases.mountain, img_decals.mountain },
        { Biome::Sea,      img_bases.sea,      img_decals.sea },
        { Biome::River,    img_bases.river,    img_decals.river },
        { Biome::Beach,    img_bases.beach,    img_decals.beach }
    };
    const int info_count = static_cast<int>(info.size());

    for (int i = 0; i < info_count; ++i) {
        const int idx = TC::ToInt(std::get<0>(info[i]));
        auto& texture = reference_textures_[idx];
        auto& decals = reference_decals_[idx];

        texture = image_io_->Load(std::get<1>(info[idx]));
        if (!texture.Size()) {
            throw LogicException("Can't load image '" + std::get<1>(info[idx]) + "'.");
        }

        const auto& decal_list = std::get<2>(info[idx]);
        const int decals_count = settings_.texture.images.decals.enabled ?
                                 static_cast<int>(decal_list.size()) :
                                 0;
        decals.resize(decals_count);
        for (int j = 0; j < decals_count; ++j) {
            decals[j] = image_io_->Load(decal_list[j]);
            if (!decals[j].Size()) {
                throw LogicException("Can't load image '" + decal_list[j] + "'.");
            }
        }

        int resolution = texture.Width();
        if (!settings_.texture.chunks_enabled &&
            settings_.texture.minimap.enabled) {
            const bool clear_edges = settings_.texture.minimap.clear_edges;
            int scale_coef = resolution / tile_size;
            if (!scale_coef) {
                ScaleUpImage(texture, tile_size / resolution);
                resolution = texture.Width();
                scale_coef = 1;
            } else {
                ScaleDownImage(texture, scale_coef, clear_edges);
                resolution /= scale_coef;
            }

            for (int j = 0; j < decals_count; ++j) {
                ScaleDownImage(decals[j], scale_coef, clear_edges);
            }
        }

        auto& heights = reference_textures_heights_[idx];
        heights.Resize(resolution, resolution);
        for (int x = 0; x < resolution; ++x) {
            for (int y = 0; y < resolution; ++y) {
                heights(x, y) = static_cast<float>(GetHeight(texture(x, y)));
            }
        }
    }
}

void TextureModule::InitAlphaBands() {
    const float bandwidth = settings_.texture.splatting.bandwidth;
    const int last_idx = kAlphaBandAccuracy - 1;
    int sea_pos = static_cast<int>(*sea_level_ * last_idx);
    int beach_pos = static_cast<int>(*beach_level_ * last_idx);
    int mountain_pos = static_cast<int>(settings_.basis.height * last_idx);

    const vector<tuple<Biome, int, int> > ranges = {
        { Biome::Mountain, mountain_pos, kAlphaBandAccuracy },
        { Biome::Basis,    beach_pos,    mountain_pos },
        { Biome::Beach,    sea_pos,      beach_pos },
        { Biome::Sea,      0,            sea_pos }
    };

    const int biomes_count = TC::ToInt(Biome::Count);
    alpha_bands_.resize(biomes_count);
    for (int i = 0; i < biomes_count; ++i) {
        auto& band = alpha_bands_[i];
        band.resize(kAlphaBandAccuracy, 0.0f);

        bool exists = false;
        int range_idx = 0;
        for (const auto& range : ranges) {
            if (std::get<0>(range) == TC::ToBiome(i)) {
                exists = true;
                break;
            }
            ++range_idx;
        }
        if (!exists) {
            continue;
        }

        const auto& range = ranges[range_idx];
        const int min = std::get<1>(range);
        const int max = std::get<2>(range);
        const int mid = (max + min) / 2;
        for (int j = min; j < max; ++j) {
            band[j] = 255.0f;
        }

        if (range_idx) {
            const int idx = range_idx - 1;
            AlphaIncline(ranges[idx], max, true, bandwidth, mid, band);
        }
        if (range_idx < ranges.size() - 1) {
            const int idx = range_idx + 1;
            AlphaIncline(ranges[idx], min, false, bandwidth, mid, band);
        }
    }
}

void TextureModule::InitGradient() {
    const auto& points = settings_.texture.gradient.points;
    const float opacity = settings_.texture.gradient.opacity;
    const int points_count = static_cast<int>(points.size());

    grad_table_.resize(kAlphaBandAccuracy);
    int end = 0;
    for (int i = 0; i < points_count - 1; ++i) {
        int begin = end;
        const int next_idx = i + 1;
        const float delta = points[next_idx].second - points[i].second;
        end = begin + static_cast<int>(delta * kAlphaBandAccuracy) - 1;
        if (i == points_count - 2) {
            end = kAlphaBandAccuracy;
        }

        const RgbaPixel& next = points[next_idx].first;
        const RgbaPixel& cur = points[i].first;

        for (int j = begin; j < end; ++j) {
            const int j_to_beg = j - begin;
            const int end_to_j = end - j;
            auto& pixel = grad_table_[j];
            pixel.red =   MixValue(next.red,   cur.red,   end_to_j, j_to_beg);
            pixel.green = MixValue(next.green, cur.green, end_to_j, j_to_beg);
            pixel.blue =  MixValue(next.blue,  cur.blue,  end_to_j, j_to_beg);
            pixel.alpha = MixValue(next.alpha, cur.alpha, end_to_j, j_to_beg);
            pixel.alpha = static_cast<uint8_t>(pixel.alpha * opacity);
        }
    }
}

float TextureModule::GetPixelHeight(int chunk_x, int chunk_y,
        int u, int v) const {
    const int size = settings_.general.chunk_size;
    const int chunks = settings_.general.size / size;
    const int real_x = ((chunk_x + chunks) % chunks) * size + u;
    const int real_y = ((chunk_y + chunks) % chunks) * size + v;
    return (*height_map_)(real_x, real_y);
}

Biome TextureModule::DetectBiome(float height) const {
    if (height < *sea_level_) {
        return Biome::Sea;
    } else if (height < *beach_level_) {
        return Biome::Beach;
    } else if (height < settings_.basis.height) {
        return Biome::Basis;
    } else {
        return Biome::Mountain;
    }
}

void TextureModule::ProcessTiles(int line_beg, int line_end) {
    const auto& gradient = settings_.texture.gradient;
    const int chunk_size = settings_.general.chunk_size;
    const int chunks_count = settings_.general.size / chunk_size;
    const int tile_size = settings_.texture.minimap.tile_size;

    Random rand(settings_.general.seed + line_beg);
    const int resolution = settings_.texture.gradient.opacity < 1.0f - kEps ?
                           reference_textures_[0].Width() :
                           settings_.texture.minimap.tile_size;
    Image texture(resolution, resolution);

    for (int x = line_beg; x < line_end; ++x) {
        for (int y = 0; y < chunks_count; ++y) {
            if (!gradient.enabled || gradient.opacity < 1.0f - kEps) {
                TextureSplatting(texture, x, y);
            }

            const int center_x = x * chunk_size + chunk_size / 2;
            const int center_y = y * chunk_size + chunk_size / 2;
            const float cur_height = (*height_map_)(center_x, center_y);
            const Biome cur_biome = DetectBiome(cur_height);

            AddDecal(cur_biome, texture, rand);
            if (gradient.only_minimap) {
                SaveChunk(texture, x, y);
            }
            if (gradient.enabled && gradient.opacity > kEps) {
                AddGradient(texture, x, y);
            }
            if (!gradient.only_minimap) {
                SaveChunk(texture, x, y);
            }
            if (settings_.texture.shadow.enabled) {
                AddShadow(texture, x, y);
            }
            DrawOnMinimap(texture, resolution / tile_size, x, y);
        }
    }
}

void TextureModule::TextureSplatting(Image& tex, int chunk_x, int chunk_y) {
    const float depth = settings_.texture.splatting.depth;
    const int resolution = tex.Width();
    const int ref_texture_count = static_cast<int>(reference_textures_.size());
    const int band_max = kAlphaBandAccuracy - 1;

    Array2D<float> height_sub_mask;
    Array2D<float> river_sub_mask;
    Array2D<float> mountain_sub_mask;
    PrepareSplattingMasks(height_sub_mask, river_sub_mask, mountain_sub_mask,
                          chunk_x, chunk_y, resolution);

    Random rand(settings_.general.seed * chunk_x + chunk_y);
    for (int x = 0; x < resolution; ++x) {
        for (int y = 0; y < resolution; ++y) {
            vector<float> heights(ref_texture_count);
            const float real_height = height_sub_mask(x, y);
            float max_height = 0;
            for (int n = 0; n < ref_texture_count; ++n) {
                const int band_idx = static_cast<int>(band_max * real_height);
                const float band = alpha_bands_[n][band_idx];
                const float height = reference_textures_heights_[n](x, y);
                heights[n] = height * band / 255.0f;
                if (heights[n] > max_height) {
                    max_height = heights[n];
                }
            }
            RgbaPixel pixel = MixPixelByAlphaBands(heights, depth,
                                                   max_height, x, y);

            const float mountain_coef = mountain_sub_mask(x, y);
            if (mountain_coef > kEps) {
                const int idx = TC::ToInt(Biome::Mountain);
                const auto& mountain_pixel = reference_textures_[idx](x, y);
                pixel = OverlayExceptSea(pixel, mountain_pixel, real_height,
                    mountain_coef, rand);
            }

            const float river_coef = river_sub_mask(x, y);
            if (river_coef > kEps) {
                const int idx = TC::ToInt(Biome::River);
                const auto& river_pixel = reference_textures_[idx](x, y);
                pixel = OverlayExceptSea(pixel, river_pixel, real_height,
                    river_coef, rand);
            }
            tex(x, y) = pixel;
        }
    }
}

void TextureModule::AddDecal(Biome biome, Image& texture,
        Random& rand) {
    const auto& decals = settings_.texture.images.decals;
    const auto& biome_decals = reference_decals_[TC::ToInt(biome)];
    const int count = static_cast<int>(biome_decals.size());
    if (decals.enabled && count) {
        const float chance = rand.Next(0.0f, 1.0f);
        if (chance <= decals.chance) {
            int decal_idx = static_cast<int>(chance * RAND_MAX);
            decal_idx %= count;
            AlphaBlendImage(texture, biome_decals[decal_idx], texture);
        }
    }
}

void TextureModule::AddGradient(Image &tex, int x, int y) const {
    Random rand(settings_.general.seed + x + y);
    const int texture_size = tex.Width();
    const int chunk_size = settings_.general.chunk_size;
    const int last_idx = kAlphaBandAccuracy - 1;
    const float px_per_edge = static_cast<float>(texture_size) / chunk_size;

    for (int u = 0; u < texture_size; ++u) {
        for (int v = 0; v < texture_size; ++v) {
            const int u_pos = static_cast<int>(u / px_per_edge);
            const int v_pos = static_cast<int>(v / px_per_edge);
            const int real_u = x * chunk_size + u_pos;
            const int real_v = y * chunk_size + v_pos;

            float height = (*height_map_)(real_u, real_v);
            float noise = settings_.texture.splatting.randomness * height;
            float add = rand.Next(-noise, noise);
            height = std::min(1.0f, std::max(0.0f, height + add));

            const int grad_idx = static_cast<int>(height * last_idx);
            auto& pixel = tex(u, v);
            pixel = AlphaBlendPixel(pixel, grad_table_[grad_idx]);
        }
    }
}

void TextureModule::AddShadow(Image& texture, int x, int y) {
    const float strength = settings_.texture.shadow.strength;
    const int angle = settings_.texture.shadow.angle % 360;

    const int chunk_size = settings_.general.chunk_size;
    const int resolution = texture.Width();
    const float px_per_edge = static_cast<float>(resolution) / chunk_size;

    RgbaPixel pixel(0, 0, 0, 0);
    for (int u = 0; u < resolution; ++u) {
        for (int v = 0; v < resolution; ++v) {
            const int real_u = static_cast<int>(u / px_per_edge);
            const int real_v = static_cast<int>(v / px_per_edge);

            float h_this =  GetPixelHeight(x,     y,     real_u, real_v);
            float h_top =   GetPixelHeight(x,     y - 1, real_u, real_v);
            float h_bot =   GetPixelHeight(x,     y + 1, real_u, real_v);
            float h_left =  GetPixelHeight(x - 1, y,     real_u, real_v);
            float h_right = GetPixelHeight(x + 1, y,     real_u, real_v);

            float alpha = 0.0f;
            switch (angle) {
            case 0:   alpha = h_right; break;
            case 45:  alpha = (h_right + h_top) / 2; break;
            case 90:  alpha = h_top; break;
            case 135: alpha = (h_left + h_top) / 2; break;
            case 180: alpha = h_left; break;
            case 225: alpha = (h_left + h_bot) / 2; break;
            case 270: alpha = h_bot; break;
            case 315: alpha = (h_right + h_bot) / 2; break;
            default:  alpha = (h_right + h_bot) / 2; break;
            }

            alpha -= h_this;
            alpha = std::max(0.0f, std::min(1.0f, alpha * strength));
            pixel.alpha = static_cast<uint8_t>(alpha * 255.0f);
            auto& texture_pixel = texture(u, v);
            texture_pixel = AlphaBlendPixel(texture_pixel, pixel);
        }
    }
}

void TextureModule::DrawOnMinimap(Image texture, int scale,
        int chunk_x, int chunk_y) {
    if (settings_.texture.minimap.enabled) {
        const int tile_size = settings_.texture.minimap.tile_size;
        if (scale > 1) {
            const bool clear_edges = settings_.texture.minimap.clear_edges;
            ScaleDownImage(texture, scale, clear_edges);
        }
        for (int x = 0; x < tile_size; ++x) {
            for (int y = 0; y < tile_size; ++y) {
                const int real_x = chunk_x * tile_size + x;
                const int real_y = chunk_y * tile_size + y;
                minimap_(real_x, real_y) = texture(x, y);
            }
        }
    }
}

void TextureModule::PrepareSplattingMasks(Array2D<float>& height_sub_mask,
        Array2D<float>& river_sub_mask, Array2D<float>& mountain_sub_mask,
        int chunk_x, int chunk_y, int resolution) const {
    const int chunk_size = settings_.general.chunk_size;
    const float randomness = settings_.texture.splatting.randomness;

    height_sub_mask.Resize(chunk_size, chunk_size);
    river_sub_mask.Resize(chunk_size, chunk_size);
    mountain_sub_mask.Resize(chunk_size, chunk_size);

    const int x_beg = chunk_x * chunk_size;
    const int y_beg = chunk_y * chunk_size;
    for (int x = 0; x < chunk_size; ++x) {
        for (int y = 0; y < chunk_size; ++y) {
            const int mask_x = x_beg + x;
            const int mask_y = y_beg + y;
            height_sub_mask(x, y) =      (*height_map_)(mask_x, mask_y);
            river_sub_mask(x, y) =       (*river_mask_)(mask_x, mask_y);
            mountain_sub_mask(x, y) = (*mountain_mask_)(mask_x, mask_y);
        }
    }
    const int scale = resolution / chunk_size;
    AT::ScaleUp(height_sub_mask, scale);
    AT::Smooth(height_sub_mask, scale * 2);
    if (randomness > kEps) {
        Array2D<float> random_mask;
        AT::WhiteNoise(random_mask, resolution,
                       settings_.general.seed * chunk_x + chunk_y);
        AT::ToRange(random_mask, -randomness, randomness);
        AT::ApplyFilter(height_sub_mask, Operation::Add,
                      height_sub_mask, random_mask);
        for (auto& elem : height_sub_mask) {
            elem = std::min(1.0f, std::max(0.0f, elem));
        }
    }
    AT::ScaleUp(river_sub_mask, scale);
    AT::Smooth(river_sub_mask, scale * 2);
    AT::ScaleUp(mountain_sub_mask, scale);
    AT::Smooth(mountain_sub_mask, scale * 2);
}

RgbaPixel TextureModule::MixPixelByAlphaBands(const vector<float>& heights,
        float depth, float max_h, int u, int v) const {
    float r = 0;
    float g = 0;
    float b = 0;
    float a = 0;
    float sum_koef = 0;
    const float delta = depth * max_h;
    const int heights_count = static_cast<int>(heights.size());
    for (int n = 0; n < heights_count; ++n) {
        if (heights[n] > max_h - delta) {
            const float band = (heights[n] - max_h + delta) / delta;
            const RgbaPixel& cur_pixel = reference_textures_[n](u, v);
            r += cur_pixel.red *   band;
            g += cur_pixel.green * band;
            b += cur_pixel.blue *  band;
            a += cur_pixel.alpha * band;
            sum_koef += band;
        }
    }
    RgbaPixel pixel;
    pixel.red =   static_cast<uint8_t>(r / sum_koef);
    pixel.green = static_cast<uint8_t>(g / sum_koef);
    pixel.blue =  static_cast<uint8_t>(b / sum_koef);
    pixel.alpha = static_cast<uint8_t>(a / sum_koef);
    return pixel;
}

RgbaPixel TextureModule::OverlayExceptSea(const RgbaPixel& pixel,
        const RgbaPixel& overlay, float height, float coef,
        Random& rand) const {
    const int sea_idx = static_cast<int>((kAlphaBandAccuracy - 1) * height);
    const float randomness = settings_.texture.splatting.randomness;

    float alpha = coef - alpha_bands_[TC::ToInt(Biome::Sea)][sea_idx];
    alpha += rand.Next(-randomness, randomness) * 255.0f;
    alpha = std::max(0.0f, std::min(255.0f, alpha));

    RgbaPixel overlay_copy = overlay;
    overlay_copy.alpha = static_cast<uint8_t>(alpha * overlay.alpha / 255.0f);
    return AlphaBlendPixel(pixel, overlay_copy);
}

void TextureModule::SaveHeightMap() {
    ImageIOParams params;
    params.filename = settings_.names.heightmap;
    params.bit_depth = settings_.texture.target_bitdepth;
    params.format = settings_.system.extensions.image;
    params.quality = 0;
    image_io_->SaveHeightMap(*height_map_, params);
}

void TextureModule::SaveChunk(const Image &tex, int x, int y) const {
    const auto& names = settings_.names;
    const auto& texture = settings_.texture;
    const auto& normals = texture.normals;

    if (texture.chunks_enabled) {
        ImageIOParams params;
        params.filename = names.texture.Apply(x, y);
        params.format = settings_.system.extensions.image;
        params.bit_depth = 24;
        params.quality = 0;
        image_io_->Save(tex, params);
        if (normals.enabled) {
            Image normal_map;
            CreateNormal(tex, normals.invert, normals.strength, normal_map);
            params.filename = names.normal.Apply(x, y);
            image_io_->Save(normal_map, params);
        }
    }
}

void TextureModule::ScaleUpImage(Image& texture, int n) {
    if (n < 2) {
        return;
    }
    const int width = texture.Width();
    const int height = texture.Height();

    Image out(width * n, height * n);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const auto& val = texture(x, y);
            const int x_start = x * n;
            const int y_start = y * n;
            Range r(x_start, x_start + n, y_start, y_start + n);
            for (int i = r.left; i < r.right + n; ++i) {
                for (int j = r.top; j < r.bottom + n; ++j) {
                    out(i, j) = val;
                }
            }
        }
    }
    texture = out;
}

void TextureModule::ScaleDownImage(Image& texture, int n, bool sharp) {
    const int size_out = texture.Width() / n;
    Image texture_out(size_out, size_out);
    for (int x = 0; x < size_out; ++x) {
        for (int y = 0; y < size_out; ++y) {
            if (sharp) {
                texture_out(x, y) = texture(x * n, y * n);
            } else {
                texture_out(x, y) = ScaleDownPixel(texture, x, y, n);
            }
        }
    }
    texture = texture_out;
}

inline int TextureModule::GetHeight(const RgbaPixel& px) {
    const int h = (307 * px.red + 602 * px.green + 115 * px.blue) >> 10;
    return h;
}

inline uint8_t TextureModule::MixValue(uint8_t v1, uint8_t v2,
        int dist_1, int dist_2) {
    const int result = (v1 * dist_2 + v2 * dist_1) / (dist_1 + dist_2);
    return static_cast<uint8_t>(result);
}

inline RgbaPixel TextureModule::ScaleDownPixel(const Image& texture,
        int x, int y, int n) {
    const int xl = x * n;
    const int xr = xl + n - 1;
    const int yt = y * n;
    const int yb = yt + n - 1;

    const auto& lt = texture(xl, yt);
    const auto& rt = texture(xr, yt);
    const auto& lb = texture(xl, yb);
    const auto& rb = texture(xr, yb);
    const int r = (lt.red +   rt.red +   lb.red +   rb.red) >> 2;
    const int g = (lt.green + rt.green + lb.green + rb.green) >> 2;
    const int b = (lt.blue +  rt.blue +  lb.blue +  rb.blue) >> 2;
    const int a = (lt.alpha + rt.alpha + lb.alpha + rb.alpha) >> 2;

    RgbaPixel pixel;
    pixel.red =   static_cast<uint8_t>(r);
    pixel.green = static_cast<uint8_t>(g);
    pixel.blue =  static_cast<uint8_t>(b);
    pixel.alpha = static_cast<uint8_t>(a);
    return pixel;
}

void TextureModule::AlphaBlendImage(const Image& back, const Image& front,
        Image& image_out) {
    image_out = back;
    const int width =  std::min(back.Width(),  front.Width());
    const int height = std::min(back.Height(), front.Height());
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            image_out(x, y) = AlphaBlendPixel(back(x, y), front(x, y));
        }
    }
}

RgbaPixel TextureModule::AlphaBlendPixel(const RgbaPixel& bot,
        const RgbaPixel& top) {
    RgbaPixel out(0, 0, 0, 0);
    int alpha = top.alpha + bot.alpha * (255 - top.alpha) / 255;
    out.alpha = static_cast<uint8_t>(alpha);
    if (out.alpha) {
        out.red = CalcComponent(bot.red, bot.alpha,
                                top.red, top.alpha, alpha);
        out.green = CalcComponent(bot.green, bot.alpha,
                                  top.green, top.alpha, alpha);
        out.blue = CalcComponent(bot.blue, bot.alpha,
                                 top.blue, top.alpha, alpha);
    }
    return out;
}

inline uint8_t TextureModule::CalcComponent(int bot_c, int bot_a,
    int top_c, int top_a, int res_a) {
    int comp = (255 * top_a * top_c + (255 - top_a) * bot_a * bot_c) /
               (255 * res_a);
    return static_cast<uint8_t>(comp);
}

void TextureModule::AlphaIncline(const tuple<Biome, int, int>& range,
        int border, bool is_decreasing, float bandwidth,
        int mid, vector<float>& band) {
    const int near_mid = (std::get<1>(range) + std::get<2>(range)) / 2;
    const int b1 = border - static_cast<int>((border - mid) * bandwidth);
    const int b2 = border + static_cast<int>((near_mid - border) * bandwidth);
    const int min = is_decreasing ? b1 : b2;
    const int max = is_decreasing ? b2 : b1;
    const int incline_size = max - min;
    for (int j = min; j < max; ++j) {
        if (is_decreasing) {
            band[j] = static_cast<float>(max - j) / incline_size * 255.0f;
        } else {
            band[j] = static_cast<float>(j - min) / incline_size * 255.0f;
        }
    }
}

void TextureModule::CreateNormal(const Image& img, bool invert,
        float coef, Image& normal) {
    const int size = img.Width();

    if (normal.Width() != size || normal.Height() != size) {
        normal.Resize(size, size);
    }
    if (invert) {
        coef *= -1;
    }

    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            Range r((x - 1 + size) % size, (x + 1) % size,
                    (y - 1 + size) % size, (y + 1) % size);

            const float dx = (GetHeight(img(r.left,  y)) -
                              GetHeight(img(r.right, y))) * coef;
            const float dy = (GetHeight(img(x,       r.top)) -
                              GetHeight(img(x,       r.bottom))) * coef;
            const float dz = 255.0f;
            const float len = std::sqrt(dx * dx + dy * dy + dz * dz);

            RgbaPixel& pixel = normal(x, y);
            pixel.red =   static_cast<uint8_t>(128.0f + 127.0f * dx / len);
            pixel.green = static_cast<uint8_t>(128.0f - 127.0f * dy / len);
            pixel.blue =  static_cast<uint8_t>(128.0f + 127.0f * dz / len);
            pixel.alpha = 255;
        }
    }
}

} // namespace modules
} // namespace prowogene
