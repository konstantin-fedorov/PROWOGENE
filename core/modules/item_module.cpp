#include "item.h"

#include "utils/array2d_tools.h"
#include "utils/types_converter.h"

namespace prowogene {
namespace modules {
    
using std::list;
using std::pair;
using std::string;
using std::vector;
using utils::Array2D;
using utils::Random;
using utils::Range;
using utils::JsonValue;
using AT = utils::Array2DTools;
using TC = utils::TypesConverter;

void ItemModule::Init() {
    const int size = settings_.general.size;
    object_mask_.Resize(size, size);
    rand_ = Random(settings_.general.seed);
}

bool ItemModule::Process() {
    if (!settings_.item.enabled) {
        return true;
    }

    const vector<pair<ImportItemList, Location> > item_lists = {
        { settings_.item.list.forest,   Location::Forest },
        { settings_.item.list.glade,    Location::Glade },
        { settings_.item.list.mountain, Location::Mountain },
        { settings_.item.list.river,    Location::River },
        { settings_.item.list.beach,    Location::Beach },
        { settings_.item.list.sea,      Location::Sea }
    };

    for (const auto& item_list : item_lists) {
        const bool success = PlaceLocation(item_list.first, item_list.second);
        if (!success) {
            status_ = "Can't place items.";
            return false;
        }
    }
    AT::Smooth(*height_map_, 1, settings_.system.thread_count);

    ExportWorldSettings info = CreateExportInfo();
    const JsonValue config = info.Serialize();
    config.Save(settings_.item.config.file, settings_.item.config.pretty);

    return true;
}

list<string> ItemModule::GetNeededSettings() const {
    return {
        kConfigGeneral,
        kConfigItem,
        kConfigLocation,
        kConfigModel,
        kConfigNames,
        kConfigSystem,
        kConfigTexture
    };
}

void ItemModule::ApplySettings(ISettings* settings) {
    CopySettings(settings, settings_.general);
    CopySettings(settings, settings_.item);
    CopySettings(settings, settings_.location);
    CopySettings(settings, settings_.model);
    CopySettings(settings, settings_.names);
    CopySettings(settings, settings_.system);
    CopySettings(settings, settings_.texture);
}

void ItemModule::Deinit() {
    object_mask_.Clear();
    placed_objects_.clear();
}

std::string ItemModule::GetName() const {
    return "Item";
}

bool ItemModule::PlaceLocation(const ImportItemList& info, Location loc) {
    if (!info.size()) {
        return true;
    }

    const int size = settings_.general.size;
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            object_mask_(x, y) = (*location_map_)(x, y) == loc ? -1 : 0;
        }
    }

    ImportItemList required_info;
    ImportItemList optional_info;
    SeparateRequiredAndOptional(info, required_info, optional_info);
    const bool success = PlaceRequired(required_info);
    if (!success) {
        return false;
    }
    PlaceOptional(optional_info);
    return true;
}

void ItemModule::ClearBorders() {
    const int size = settings_.general.size;
    for (int i = 0; i < size; ++i) {
        object_mask_(i,        0       ) = 0;
        object_mask_(0,        i       ) = 0;
        object_mask_(i,        size - 1) = 0;
        object_mask_(size - 1, i       ) = 0;
    }
}

void ItemModule::CreateWave() {
    const int size = settings_.general.size;

    bool was_changed = true;
    int cur_wave = 1;
    while (was_changed) {
        was_changed = false;
        ClearBorders();

        const int prev_wave = cur_wave - 1;
        for (int x = 1; x < size - 1; ++x) {
            for (int y = 1; y < size - 1; ++y) {
                if ((object_mask_(x, y) == -1) && (
                        object_mask_(x + 1, y    ) == prev_wave ||
                        object_mask_(x - 1, y    ) == prev_wave ||
                        object_mask_(x,     y + 1) == prev_wave ||
                        object_mask_(x,     y - 1) == prev_wave)) {
                    object_mask_(x, y) = cur_wave;
                    was_changed = true;
                }
            }
        }
        ++cur_wave;
    }
}

void ItemModule::UpdateWave(int x_center, int y_center,
        int item_radius, int old_val) {
    Range r(x_center - item_radius, x_center + item_radius,
            y_center - item_radius, y_center + item_radius);

    const int max_wave = old_val - item_radius;
    for (int i = 1; i < max_wave; ++i) {
        for (int j = r.top; j <= r.bottom; ++j) {
            auto& left_elem =  object_mask_(r.left, j);
            auto& right_elem = object_mask_(r.right, j);
            left_elem = std::min(left_elem, i);
            right_elem = std::min(right_elem, i);
        }
        for (int j = r.left; j <= r.right; ++j) {
            auto& top_elem =    object_mask_(j, r.top);
            auto& bottom_elem = object_mask_(j, r.bottom);
            top_elem = std::min(top_elem, i);
            bottom_elem = std::min(bottom_elem, i);
        }
        --r.left;
        ++r.right;
        --r.top;
        ++r.bottom;
    }
}

void ItemModule::SeparateRequiredAndOptional(const ImportItemList& all,
        ImportItemList& required, ImportItemList& optional) {
    const int size = settings_.general.size;
    const float edge_size = settings_.model.edge_size;
    const float mask_radius = static_cast<float>(size / 2) * edge_size;

    required.reserve(all.size());
    optional.reserve(all.size());

    for (auto item : all) {
        if (!item.count.max || item.count.min < 0) {
            continue;
        }
        if (item.count.max < 0) {
            if (!item.count.min) {
                optional.push_back(item);
            } else {
                item.count.max = item.count.min;
                optional.push_back(item);
                required.push_back(item);
            }
        } else {
            if (!item.count.max ||
                    item.radius > mask_radius ||
                    item.count.min > item.count.max) {
                continue;
            }
            const int count = rand_.Next(item.count.min, item.count.max);
            if (!count) {
                continue;
            }
            item.count.min = count;
            item.count.max = count;
            required.push_back(item);
        }
    }
}

bool ItemModule::PlaceRequired(const ImportItemList& required_info) {
    if (!required_info.size()) {
        return true;
    }

    const int size = settings_.general.size;
    const float edge_size = settings_.model.edge_size;

    CreateWave();

    for (const auto& item : required_info) {
        const int count = item.count.min;
        const int size_in_points = static_cast<int>(item.radius / edge_size);
        for (int n = 0; n < count; ++n) {
            Point<int> max;
            Point<int> min;
            AT::FindMinMaxInArea(min, max, object_mask_, 0, 0, size, size);

            if (max.value < size_in_points) {
                return false;
            }
            PlaceItem(item, max, true);
            UpdateWave(max.x, max.y, size_in_points, max.value);
        }
    }
    return true;
}

void ItemModule::PlaceOptional(const ImportItemList& optional) {
    if (!optional.size()) {
        return;
    }

    const float edge = settings_.model.edge_size;
    const float map_length = settings_.general.size * edge;
    const int   list_size = static_cast<int>(optional.size());
    const int   size = settings_.general.size;;

    vector<float> chances;
    chances.resize(list_size);
    chances[0] = optional[0].frequency;
    for (int n = 1; n < list_size; ++n) {
        const int prev_idx = n - 1;
        chances[n] = chances[prev_idx] + optional[n].frequency;
    }
    const float chance_max = chances.back();
    for (int n = 0; n < list_size; ++n) {
        chances[n] /= chance_max;
    }

    const float fullness = settings_.item.fullness;
    const int points_count = static_cast<int>(size * size * fullness);
    for (int i = 0; i < points_count; ++i) {
        const float item_chance = rand_.Next(0.0f, 1.0f);
        int cur_item_idx = 0;
        for (int n = 0; n < list_size; ++n) {
            if (item_chance > chances[n]) {
                cur_item_idx = n;
            } else {
                break;
            }
        }
        const auto& item = optional[cur_item_idx];
        const float item_radius_in_mask = item.radius / edge + 1.0f;
        const int item_size = static_cast<int>(item_radius_in_mask * 2.0f);

        const int max_coord = size - item_size > 0 ? size - item_size : 0;
        const int x = rand_.Next(0, max_coord);
        const int y = rand_.Next(0, max_coord);

        bool need_add = true;
        const int x_right =  x + item_size;
        const int y_bottom = y + item_size;
        for (int k = x; k < x_right; k++) {
            for (int l = y; l < y_bottom; l++) {
                if (!object_mask_(k, l)) {
                    need_add = false;
                    break;
                }
            }
            if (!need_add) {
                break;
            }
        }

        if (need_add) {
            Point<int> center;
            center.x = x + item_size / 2;
            center.y = y + item_size / 2;
            PlaceItem(item, center, false);
        }
    }
}

void ItemModule::PlaceItem(const ImportItemSettings& item,
        const Point<int>& center, const bool clean_mask) {
    const float edge = settings_.model.edge_size;
    const int   size = settings_.general.size;
    const float map_length = size * edge;
    const float map_center = map_length / 2.0f;
    const float map_height = settings_.model.map_height;

    const float height = (*height_map_)(center.x, center.y);

    ExportItemSettings setting;
    setting.file = item.file;
    setting.id = item.id;
    setting.x = center.x * edge - map_center;
    setting.y = map_center - center.y * edge;
    setting.z = height * map_height;
    setting.angle = rand_.Next(static_cast<float>(item.angle.min),
                               static_cast<float>(item.angle.max));
    placed_objects_.push_back(setting);

    if (clean_mask) {
        const int size_in_mask = static_cast<int>(item.radius / edge);
        const Range r(center.x - size_in_mask, center.x + size_in_mask + 1,
                      center.y - size_in_mask, center.y + size_in_mask + 1);
        for (int x = r.left; x < r.right; ++x) {
            for (int y = r.top; y < r.bottom; ++y) {
                object_mask_(x, y) = 0;
            }
        }
    }

    if (item.leveled) {
        const float x_coord = setting.x / edge;
        const float y_coord = setting.y / edge;
        const Range r(static_cast<int>(x_coord - item.radius),
                      static_cast<int>(x_coord + item.radius),
                      static_cast<int>(y_coord - item.radius),
                      static_cast<int>(y_coord + item.radius));
        for (int hm_x = r.left; hm_x < r.right; ++hm_x) {
            for (int hm_y = r.top; hm_y < r.bottom; ++hm_y) {
                (*height_map_)(hm_x, hm_y) = setting.z / map_height;
            }
        }
    }
}

ExportWorldSettings ItemModule::CreateExportInfo() {
    const int   size = settings_.general.size;
    const int   chunk_size = settings_.general.chunk_size;
    const float real_chunk_size = chunk_size * settings_.model.edge_size;
    const float complex_size = size * settings_.model.edge_size;
    const float complex_half_size = complex_size / 2.0f;

    ExportWorldSettings world_info;
    world_info.water_level = *sea_level_ * settings_.model.map_height - kEps;

    world_info.complex.size = complex_size;
    if (settings_.model.complex_enabled) {
        const auto& minimap = settings_.names.minimap;
        const bool minimap_enabled = settings_.texture.minimap.enabled;
        FillExportFiles(world_info.complex.files, minimap.model,
            minimap_enabled, minimap.texture, minimap.normal);
        for (auto& item : placed_objects_) {
            world_info.complex.items.push_back(item);
        }
    }

    if (!settings_.model.chunks_enabled) {
        return world_info;
    }

    world_info.chunks.size = real_chunk_size;
    world_info.chunks.count_x = size / chunk_size;
    world_info.chunks.count_y = size / chunk_size;
    for (int x = 0; x < world_info.chunks.count_x; ++x) {
        for (int y = 0; y < world_info.chunks.count_y; ++y) {
            ExportChunkSettings chunk_info;
            const auto& names = settings_.names;
            const bool chunks_enabled = settings_.texture.chunks_enabled;
            FillExportFiles(chunk_info.info.files, names.chunk.Apply(x, y),
                chunks_enabled, names.texture.Apply(x, y),
                names.normal.Apply(x, y));
            chunk_info.info.x = x;
            chunk_info.info.y = y;

            for (auto& item : placed_objects_) {
                const int item_chunk_x = static_cast<int>(
                    (item.x + complex_half_size) / real_chunk_size);
                const int item_chunk_y = static_cast<int>(
                    (item.y + complex_half_size) / real_chunk_size);
                if (item_chunk_x != x || item_chunk_y != y) {
                    continue;
                }
                chunk_info.items.push_back(item);
            }
            world_info.chunks.data.push_back(chunk_info);
        }
    }

    return world_info;
}

void ItemModule::FillExportFiles(ExportFiles& files, const string& model,
        const bool texture_enabled, const string& texture,
        const string& normal) const {
    const string model_ext = settings_.system.extensions.model;
    const string image_ext = settings_.system.extensions.image;

    files.model = model + "." + model_ext;
    if (texture_enabled) {
        files.texture = texture + "." + image_ext;
    }
    if (settings_.texture.normals.enabled) {
        files.normal = normal + "." + image_ext;
    }
}

} // namescpace modules
} // namescpace prowogene
