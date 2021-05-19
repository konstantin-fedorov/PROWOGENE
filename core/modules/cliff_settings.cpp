#include "cliff.h"

namespace prowogene {
namespace modules {

using std::string;
using utils::JsonObject;

static const string kEnabled = "enabled";
static const string kOctaves = "octaves";
static const string kLevels =  "levels";
static const string kSeed =    "seed";
static const string kGrain =   "grain";

void CliffSettings::Deserialize(JsonObject config) {
    enabled = config[kEnabled];
    octaves = config[kOctaves];
    levels =  config[kLevels];
    seed =    config[kSeed];
    grain =   config[kGrain];
}

JsonObject CliffSettings::Serialize() const {
    JsonObject config;
    config[kEnabled] = enabled;
    config[kOctaves] = octaves;
    config[kLevels] =  levels;
    config[kSeed] =    seed;
    config[kGrain] =   grain;
    return config;
}

string CliffSettings::GetName() const {
    return kConfigCliff;
}

void CliffSettings::Check() const {
    CheckCondition(octaves > 0, "octaves is less than 1");
    CheckCondition(levels > 1,  "levels is less than 2");
    CheckCondition(seed >= 0,   "seed is less than 1");
    CheckInRange(grain, 0.f, 1.f, "grain");
}

} // namespace modules
} // namespace prowogene
