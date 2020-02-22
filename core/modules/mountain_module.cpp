#include "mountain.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "utils/array2d_tools.h"

namespace prowogene {
namespace modules {

using std::list;
using std::string;
using utils::Array2D;
using utils::Random;
using AT = utils::Array2DTools;

void MountainModule::SetStorage(Storage* storage) {
    LinkData(height_map_,    storage, kStorageHeightMap);
    LinkData(mountain_mask_, storage, kStorageMountainMask);
    LinkData(location_map_,  storage, kStorageLocationMap);
}

bool MountainModule::Process() {
    if (settings_.mountain.count < 1) {
        return true;
    }

    const int thread_count = settings_.system.thread_count;
    const int size = settings_.general.size;

    if (mountain_mask_->Width() != size ||
            mountain_mask_->Height() != size) {
        mountain_mask_->Resize(size, size, 0.0f);
    }

    Random rand(settings_.general.seed);
    Array2D<float> ridge(size, size, 0.0f);
    Array2D<float> mountain;

    for (int i = 0; i < settings_.mountain.count; ++i) {
        SingleMountainSettings& mountain_settings =
            settings_.mountain.settings[i];
        if (mountain_settings.size > size) {
            return false;
        }

        Mountain(mountain, mountain_settings, rand.Next(), thread_count);
        AT::ChangeRes(mountain, size, size, 0.0f);
        AT::SetAlign(mountain, KeyPoint::Max, mountain_settings.align,
            i * (settings_.general.seed + i));
        AT::ToRange(mountain, 0.0f, mountain_settings.height, thread_count);
        AT::ApplyFilter(ridge, Operation::Max, ridge, mountain, thread_count);
    }

    MarkMountains(ridge);

    return true;
}

list<string> MountainModule::GetNeededData() const {
    return {
        kStorageHeightMap,
        kStorageMountainMask,
        kStorageLocationMap
    };
}

list<string> MountainModule::GetNeededSettings() const {
    return {
        kConfigBasis,
        kConfigGeneral,
        kConfigLocation,
        kConfigMountain,
        kConfigSystem
    };
}

void MountainModule::ApplySettings(ISettings *settings) {
    CopySettings(settings, settings_.basis);
    CopySettings(settings, settings_.general);
    CopySettings(settings, settings_.location);
    CopySettings(settings, settings_.mountain);
    CopySettings(settings, settings_.system);
}

std::string MountainModule::GetName() const {
    return "Mountain";
}

void MountainModule::Mountain(Array2D<float> &mountain,
        const SingleMountainSettings& settings, int seed, int thread_count) {
    const int size = settings.size;
    mountain.Resize(size, size, 1.0f);

    Array2D<float> buffer(size, size);
    Random rand(seed);
    for (int i = 0; i < settings.noises_count; ++i) {
        AT::DiamondSquare(buffer, size, rand.Next(), 1, 0.0f, 1.0f);
        AT::SetAlign(buffer, KeyPoint::Max, Align::Center);
        AT::ApplyFilter(mountain, Operation::Min, mountain, buffer,
                        thread_count);
    }

    AT::ToRange(mountain, 0.0f, 1.0f, thread_count);
    Array2D<float> cone(size, size);
    AT::RadialGradient(cone, size, settings.gradient);
    AT::ApplyFilter(mountain, Operation::Multiply, mountain, cone,
                    thread_count);

    AT::ApplyGradient(mountain, settings.hillside);

    if (settings.mouth.enabled) {
        AddMouth(mountain, settings, seed, thread_count);
    }
}

void MountainModule::AddMouth(Array2D<float>& mountain,
        const SingleMountainSettings& settings, int seed, int thread_count) {
    const int size = settings.size;
    const int mouth_size = static_cast<int>(settings.mouth.width * size);

    Array2D<float> mouth(mouth_size, mouth_size);
    AT::RadialGradient(mouth, mouth_size, Gradient::Sinusoidal, size);

    Array2D<float> noise(mouth_size, mouth_size);
    AT::DiamondSquare(noise, size, seed, 1, 0.0f, 1.0f);
    AT::SetAlign(noise, KeyPoint::Max, Align::Center);
    AT::ApplyFilter(mouth, Operation::Multiply, mouth, noise, thread_count);
    AT::ToRange(mouth, 0, settings.mouth.depth, thread_count);

    Array2D<float> mask(size, size, settings.height);
    AT::ApplyFilter(mask, Operation::Substract, mask, mouth, thread_count);
    AT::ApplyFilter(mountain, Operation::Min, mountain, mask, thread_count);
}

void MountainModule::MarkMountains(const Array2D<float>& ridge) {
    const int size = settings_.general.size;

    if (location_map_->Width() != size ||
        location_map_->Height() != size) {
        location_map_->Resize(size, size);
    }

    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            const float basis_h = (*height_map_)(x, y);
            const float ridge_h = ridge(x, y);
            if (basis_h < ridge_h) {
                (*height_map_)(x, y) = ridge_h;
                (*location_map_)(x, y) = Location::Mountain;
                if (ridge_h > settings_.basis.height) {
                    (*mountain_mask_)(x, y) = 255.0f;
                } else {
                    float delta = ridge_h - basis_h;
                    delta /= settings_.basis.height;
                    (*mountain_mask_)(x, y) = delta * 255.0f;
                }
                (*location_map_)(x, y) = Location::Mountain;
            }
        }
    }
}

}  // namespace modules
}  // namespace prowogene
