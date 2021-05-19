#include "cliff.h"

#include <cmath>

#include "utils/array2d_tools.h"


namespace prowogene {
namespace modules {

using std::list;
using std::string;
using utils::Array2D;
using AT = utils::Array2DTools;

void CliffModule::Process() {
    const int size = settings_.general.size;
    const int octaves_count = settings_.cliff.octaves;
    const int levels_count = settings_.cliff.levels;
    const int seed = settings_.cliff.seed;
    const float step = 1.0f / (levels_count - 1);
    const int threads = settings_.system.thread_count;

    if (!settings_.cliff.enabled) {
        return;
    }

    if (height_map_->Width() != size ||
            height_map_->Height() != size) {
        height_map_->Resize(size, size);
    }

    Array2D<float> cliff_map(size, size);
    AT::DiamondSquare(cliff_map, size, seed, octaves_count, 0.0f, 1.0f);

    Array2D<float> noise(size, size);
    AT::WhiteNoise(noise, size, seed);
    AT::ToRange(noise, 0.0f, step * settings_.cliff.grain, threads);

    for (auto& elem : cliff_map) {
        const float scaled = elem * levels_count - step / 2.0f;
        const int level_id = static_cast<int>(std::round(scaled));
        elem = level_id * step * settings_.basis.height;
    }
    AT::ApplyFilter(cliff_map, Operation::Add, cliff_map, noise, threads);
    AT::Smooth(cliff_map, 2, threads);

    AT::ApplyFilter(*height_map_, Operation::Min, *height_map_, cliff_map,
        threads);
}

list<string> CliffModule::GetNeededSettings() const {
    return {
        settings_.basis.GetName(),
        settings_.cliff.GetName(),
        settings_.general.GetName(),
        settings_.system.GetName()
    };
}

void CliffModule::ApplySettings(ISettings* settings) {
    CopySettings(settings, settings_.basis);
    CopySettings(settings, settings_.cliff);
    CopySettings(settings, settings_.general);
    CopySettings(settings, settings_.system);
}

string CliffModule::GetName() const {
    return "Cliff";
}

} // namespace modules
} // namespace prowogene
