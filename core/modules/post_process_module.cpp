#include "post_process.h"

#include <cmath>
#include <algorithm>

#include "utils/array2d_tools.h"

namespace prowogene {
namespace modules {

using std::list;
using std::string;
using utils::Array2D;
using AT = utils::Array2DTools;

bool PostProcessModule::Process() {
    if (!settings_.postprocess.enabled) {
        return true;
    }

    const float max = settings_.postprocess.heightmap.crop_top;
    const float min = settings_.postprocess.heightmap.crop_bottom;
    const int   search_depth = settings_.system.search_depth;

    float level_top = 0.0f;
    float level_bottom = 0.0f;
    AT::GetLevel(level_top,    *height_map_, max, search_depth);
    AT::GetLevel(level_bottom, *height_map_, min, search_depth);
    
    const int sign = settings_.postprocess.heightmap.invert ? -1 : 1;
    const float power = settings_.postprocess.heightmap.power;
    for (auto& elem : *height_map_) {
        elem = sign * std::pow(elem, power);
        elem = std::min(level_top, std::max(level_bottom, elem));
    }
    return true;
}

list<string> PostProcessModule::GetNeededSettings() const {
    return {
        settings_.system.GetName(),
        settings_.postprocess.GetName()
    };
}

void PostProcessModule::ApplySettings(ISettings *settings) {
    CopySettings(settings, settings_.system);
    CopySettings(settings, settings_.postprocess);
}

std::string PostProcessModule::GetName() const {
    return "PostProcess";
}

} // namespace modules
} // namespace prowogene
