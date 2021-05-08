#include "basis.h"

#include "utils/types_converter.h"

namespace prowogene {
namespace modules {

using std::string;
using utils::JsonValue;
using utils::JsonObject;
using TC = utils::TypesConverter;

static const string kSurface =     "surface";
static const string kPeriodicity = "periodicity";
static const string kDistortion =  "distortion";
static const string kAlign =       "align";
static const string kKeyPoint =    "key_point";
static const string kHeight =      "height";

void BasisSettings::Deserialize(JsonObject config) {
    height =      config[kHeight];
    surface =     TC::To<Surface>(config[kSurface]);
    periodicity = config[kPeriodicity];
    distortion =  TC::To<Distortion>(config[kDistortion]);
    align =       TC::To<Align>(config[kAlign]);
    key_point =   TC::To<KeyPoint>(config[kKeyPoint]);
}

JsonObject BasisSettings::Serialize() const {
    JsonObject config;
    config[kHeight] =      height;
    config[kSurface] =     TC::ToString(surface);
    config[kPeriodicity] = periodicity;
    config[kDistortion] =  TC::ToString(distortion);
    config[kAlign] =       TC::ToString(align);
    config[kKeyPoint] =    TC::ToString(key_point);
    return config;
}

bool BasisSettings::IsCorrect() const {
    if (height < 0.0f || height > 1.0f || periodicity < 1) {
        return false;
    }
    return true;
}

string BasisSettings::GetName() const {
    return kConfigBasis;
}

} // namespace prowogene
} // namespace modules
