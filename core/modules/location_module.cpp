#include "location.h"

#include <cmath>

#include "utils/array2d_tools.h"
#include "utils/random.h"

namespace prowogene {
namespace modules {

using std::list;
using std::map;
using std::pair;
using std::string;
using std::vector;
using utils::Array2D;
using utils::ImageIOParams;
using utils::Random;
using utils::RgbaPixel;
using AT = utils::Array2DTools;

void LocationModule::Process() {
    if (!settings_.location.enabled) {
        return;
    }

    const int size = settings_.general.size;
    if (location_map_->Width() != size ||
            location_map_->Height() != size) {
        location_map_->Resize(size, size);
    }

    CreateBasedOnHeight();
    CreateRiverLocations();
    DivideForestAndGlade();

    if (settings_.location.map_enable)  {
        SaveMap(settings_.names.location_map.c_str());
    }
}

list<string> LocationModule::GetNeededSettings() const {
    return {
        settings_.basis.GetName(),
        settings_.general.GetName(),
        settings_.location.GetName(),
        settings_.names.GetName(),
        settings_.system.GetName()
    };
}

std::string LocationModule::GetName() const {
    return "Location";
}

void LocationModule::ApplySettings(ISettings* settings) {
    CopySettings(settings, settings_.basis);
    CopySettings(settings, settings_.general);
    CopySettings(settings, settings_.location);
    CopySettings(settings, settings_.names);
    CopySettings(settings, settings_.system);
}

void LocationModule::SaveMap(const std::string& filename) {
    const int size = location_map_->Width();
    Array2D<RgbaPixel> image(size, size);

    map<Location, RgbaPixel> colors = {
        { Location::None,     settings_.location.colors.none },
        { Location::Forest,   settings_.location.colors.forest },
        { Location::Glade,    settings_.location.colors.glade },
        { Location::Mountain, settings_.location.colors.mountain },
        { Location::River,    settings_.location.colors.river },
        { Location::Beach,    settings_.location.colors.beach },
        { Location::Sea,      settings_.location.colors.sea }
    };

    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            const auto& iter = colors.find((*location_map_)(x, y));
            if (iter != colors.end()) {
                image(x, y) = iter->second;
            }
        }
    }
    ImageIOParams params;
    params.bit_depth = 24;
    params.filename = filename;
    params.format = "bmp";
    image_io_->Save(image, params);
}

void LocationModule::CreateBasedOnHeight() {
    const int size = settings_.general.size;

    const vector<pair<Location, float> > points = {
        { Location::Forest, *beach_level_ },
        { Location::Beach,  *sea_level_ },
        { Location::Sea,     0.0f }
    };
    const int points_count = static_cast<int>(points.size());

    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            if ((*location_map_)(x, y) != Location::None) {
                continue;
            }

            const float height = (*height_map_)(x, y);
            Location location = Location::None;

            for (int i = 0; i < points_count; ++i) {
                if (height >= points[i].second) {
                    (*location_map_)(x, y) = points[i].first;
                    break;
                }
            }
        }
    }
}

void LocationModule::CreateRiverLocations() {
    const int size = settings_.general.size;

    if (river_mask_->Width() != size ||
        river_mask_->Height() != size) {
        river_mask_->Resize(size, size);
    }
    
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            if ((*river_mask_)(x, y) > kEps &&
                    (*location_map_)(x, y) != Location::Sea) {
                (*location_map_)(x, y) = Location::River;
            }
        }
    }
}

void LocationModule::DivideForestAndGlade() {
    const int size = settings_.general.size;
    const int octaves = settings_.location.forest_octaves;
    const float ratio = settings_.location.forest_ratio;

    Array2D<float> ds_noise;
    Random rand(settings_.general.seed);

    AT::DiamondSquare(ds_noise, size, rand.Next(), octaves, 0.0f, 1.0f);

    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            if ((*location_map_)(x, y) != Location::Forest)
                ds_noise(x, y) = 0.0f;
        }
    }
    const float level = GetForestLevel(ds_noise, ratio);
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            if ((*location_map_)(x, y) == Location::Forest &&
                    ds_noise(x, y) < level)
                (*location_map_)(x, y) = Location::Glade;
        }
    }
}

float LocationModule::GetForestLevel(const Array2D<float>& table,
        float ratio) {
    const int width = table.Width();
    const int height = table.Height();

    float bound_min = table(0, 0);
    float bound_max = table(0, 0);
    AT::GetMinMax(table, bound_min, bound_max);

    int total_non_zero = 0;
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if (table(x, y) >= kEps) {
                total_non_zero++;
            }
        }
    }

    float current_level = 0.0f;
    for (int i = 0; i < settings_.system.search_depth; i++) {
        int count_less = 0;
        current_level = (bound_min + bound_max) / 2;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (table(x, y) < current_level && table(x, y) > kEps) {
                    ++count_less;
                }
            }
        }

        if (static_cast<float>(count_less) / total_non_zero > ratio) {
            bound_max = current_level;
        } else {
            bound_min = current_level;
        }
    }
    return 1.0f - current_level;
}

} // namespace modules
} // namespace prowogene
