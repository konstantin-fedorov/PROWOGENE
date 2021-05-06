#include "water.h"

#include "utils/array2d_tools.h"

namespace prowogene {
namespace modules {

using utils::Array2D;
using AT = utils::Array2DTools;

bool WaterModule::Process() {
    const float sea_ratio = settings_.water.sea.ratio;
    const int search_depth = settings_.system.search_depth;
    if (settings_.water.sea.enabled) {
        AT::GetLevel(*sea_level_, *height_map_, sea_ratio, search_depth);
        if (settings_.water.sea.erosion &&
                *sea_level_ > kEps) {
            for (auto& elem : *height_map_) {
                if (elem < *sea_level_) {
                    elem *= elem;
                    elem /= *sea_level_;
                }
            }
        }
    }

    if (settings_.water.beach.enabled) {
        const float beach_ratio = settings_.water.beach.ratio;
        AT::GetLevel(*beach_level_, *height_map_, sea_ratio + beach_ratio,
            search_depth);
    }

    return true;
}

std::list<std::string> WaterModule::GetNeededSettings() const {
    return {
        kConfigSystem,
        kConfigWater
    };
}

void WaterModule::ApplySettings(ISettings* settings) {
    CopySettings(settings, settings_.system);
    CopySettings(settings, settings_.water);
}

std::string WaterModule::GetName() const {
    return "Water";
}

} // namescpace modules
} // namescpace prowogene
