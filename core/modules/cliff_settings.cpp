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

bool CliffSettings::IsCorrect() const {
    if (octaves < 1 ||
            levels < 2 ||
            seed < 0 ||
            grain < 0.0f ||
            grain > 1.0f) {
        return false;
    }
    return true;
}

} // namespace modules
} // namespace prowogene
