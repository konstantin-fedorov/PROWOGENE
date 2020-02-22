#include "types_converter.h"

#include <map>

namespace prowogene {
namespace utils {

using std::map;
using std::string;

static const string kAlignCenter =  "center";
static const string kAlignCorner =  "corner";
static const string kAlignDefault = "default";
static const string kAlignRandom =  "random";

static const string kDistortionNone =    "none";
static const string kDistortionQuadric = "quadric";

static const string kGradientLinear =     "linear";
static const string kGradientQuadric =    "quadric";
static const string kGradientSinusoidal = "sinusoidal";

static const string kKeyPointDefault = "default";
static const string kKeyPointMax =     "maximal";
static const string kKeyPointMin =     "minimal";

static const string kSurfaceDiamondSquare =  "diamond_square";
static const string kSurfaceFlat =           "flat";
static const string kSurfaceRadialGradient = "radial_gradient";
static const string kSurfaceWhiteNoise =     "white_noise";

static const map<Align, string> kAlignString = {
    { Align::Center,  kAlignCenter },
    { Align::Corner,  kAlignCorner },
    { Align::Default, kAlignDefault },
    { Align::Random,  kAlignRandom }
};

static const map<Distortion, string> kDistortionString = {
    { Distortion::None,    kDistortionNone },
    { Distortion::Quadric, kDistortionQuadric }
};

static const map<Gradient, string> kGradientString = {
    { Gradient::Linear,     kGradientLinear },
    { Gradient::Quadric,    kGradientQuadric },
    { Gradient::Sinusoidal, kGradientSinusoidal }
};

static const map<KeyPoint, string> kKeyPointString = {
    { KeyPoint::Max,     kKeyPointMax },
    { KeyPoint::Min,     kKeyPointMin },
    { KeyPoint::Default, kKeyPointDefault }
};

static const map<Surface, string> kSurfaceString = {
    { Surface::DiamondSquare,  kSurfaceDiamondSquare },
    { Surface::Flat,           kSurfaceFlat },
    { Surface::RadialGradient, kSurfaceRadialGradient },
    { Surface::WhiteNoise,     kSurfaceWhiteNoise }
};

static const map<Biome, int> kBiomeInt = {
    { Biome::Basis,    0 },
    { Biome::Mountain, 1 },
    { Biome::Sea,      2 },
    { Biome::River,    3 },
    { Biome::Beach,    4 },
    { Biome::Count,    5 },
    { Biome::Temp,     100 }
};


Align TypesConverter::ToAlign(const string &str) {
    for (const auto& elem : kAlignString) {
        if (elem.second == str) {
            return elem.first;
        }
    }
    return Align::Default;
}

Distortion TypesConverter::ToDistortion(const string& str) {
    for (const auto& elem : kDistortionString) {
        if (elem.second == str) {
            return elem.first;
        }
    }
    return Distortion::None;
}

Gradient TypesConverter::ToGradient(const string& str) {
    for (const auto& elem : kGradientString) {
        if (elem.second == str) {
            return elem.first;
        }
    }
    return Gradient::Linear;
}

KeyPoint TypesConverter::ToKeyPoint(const string &str) {
    for (const auto& elem : kKeyPointString) {
        if (elem.second == str) {
            return elem.first;
        }
    }
    return KeyPoint::Default;
}


Surface TypesConverter::ToSurface(const string& str) {
    for (const auto& elem : kSurfaceString) {
        if (elem.second == str) {
            return elem.first;
        }
    }
    return Surface::Flat;
}

string TypesConverter::ToString(Align val) {
    auto str = kAlignString.find(val);
    if (str != kAlignString.end()) {
        return str->second;
    } else {
        return "";
    }
}

string TypesConverter::ToString(Distortion val) {
    auto str = kDistortionString.find(val);
    if (str != kDistortionString.end()) {
        return str->second;
    } else {
        return "";
    }
}

string TypesConverter::ToString(Gradient val) {
    auto str = kGradientString.find(val);
    if (str != kGradientString.end()) {
        return str->second;
    } else {
        return "";
    }
}

string TypesConverter::ToString(KeyPoint val) {
    auto str = kKeyPointString.find(val);
    if (str != kKeyPointString.end()) {
        return str->second;
    } else {
        return "";
    }
}

string TypesConverter::ToString(Surface val) {
    auto str = kSurfaceString.find(val);
    if (str != kSurfaceString.end()) {
        return str->second;
    } else {
        return "";
    }
}

Biome TypesConverter::ToBiome(int val) {
    for (const auto& elem : kBiomeInt) {
        if (elem.second == val) {
            return elem.first;
        }
    }
    return Biome::Temp;
}

int TypesConverter::ToInt(Biome biome) {
    auto val = kBiomeInt.find(biome);
    if (val != kBiomeInt.end()) {
        return val->second;
    } else {
        return -1;
    }
}

} // namespace prowogene
} // namespace utils
