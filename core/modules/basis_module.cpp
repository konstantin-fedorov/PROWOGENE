#include "basis.h"

#include "utils/array2d_tools.h"

namespace prowogene {
namespace modules {

using utils::Array2D;
using AT = utils::Array2DTools;

void BasisModule::Process() {
    const int size = settings_.general.size;
    if (height_map_->Width() != size || height_map_->Height() != size) {
        height_map_->Resize(size, size);
    }

    const auto& basis = settings_.basis;
    const int seed = settings_.general.seed;
    const int threads = settings_.system.thread_count;

    AT::ApplySurface(*height_map_, basis.surface, seed, basis.periodicity);
    AT::ApplyDistortion(*height_map_, basis.distortion);
    AT::ToRange(*height_map_, 0.0f, basis.height, threads);
    AT::SetAlign(*height_map_, basis.key_point, basis.align);
}

std::list<std::string> BasisModule::GetNeededSettings() const {
    return {
        settings_.basis.GetName(),
        settings_.general.GetName(),
        settings_.system.GetName()
    };
}

void BasisModule::ApplySettings(ISettings* settings) {
    CopySettings(settings, settings_.basis);
    CopySettings(settings, settings_.general);
    CopySettings(settings, settings_.system);
}

std::string BasisModule::GetName() const {
    return "Basis";
}

} // namespace modules
} // namespace prowogene
