#include "mountain.h"

#include "utils/types_converter.h"

namespace prowogene {
namespace modules {

using std::string;
using utils::JsonValue;
using utils::JsonObject;
using utils::JsonArray;
using TC = utils::TypesConverter;

static const string kSettings = "settings";
static const string kNoises =   "noises";
static const string kGradient = "gradient";
static const string kHillside = "hillside";
static const string kAlign =    "align";
static const string kHeight =   "height";
static const string kSize =     "size";
static const string kMouth =    "mouth";
static const string kMouthEnabled = "enabled";
static const string kMouthWidth =   "width";
static const string kMouthDepth =   "depth";

void SingleMountainSettings::Deserialize(JsonObject config) {
    size =         config[kSize];
    height =       config[kHeight];
    noises_count = config[kNoises];
    gradient =     TC::To<Gradient>(config[kGradient]);
    hillside =     TC::To<Gradient>(config[kHillside]);
    align =        TC::To<Align>(config[kAlign]);
    JsonObject mouth_config = config[kMouth];
    mouth.enabled = mouth_config[kMouthEnabled];
    mouth.width =   mouth_config[kMouthWidth];
    mouth.depth =   mouth_config[kMouthDepth];
}

JsonObject SingleMountainSettings::Serialize() const {
    JsonObject config;
    config[kSize] =   size;
    config[kHeight] = height;
    config[kNoises] = noises_count;
    config[kGradient] = TC::ToString(gradient);
    config[kHillside] = TC::ToString(hillside);
    config[kAlign] =    TC::ToString(align);
    JsonObject mouth_config;
    mouth_config[kMouthEnabled] = mouth.enabled;
    mouth_config[kMouthWidth] =   mouth.width;
    mouth_config[kMouthDepth] =   mouth.depth;
    config[kMouth] = mouth_config;
    return config;
}

void SingleMountainSettings::Check() const {
    CheckCondition(size > 1,         "size is less than 2");
    CheckCondition(!(size&(size-1)), "size isn't a power of 2");
    CheckInRange(height, 0.f, 1.f, "height");
    CheckCondition(noises_count > 0, "noises_count is less than 1");
    CheckInRange(mouth.width, 0.f, 1.f, "mouth.width");
    CheckInRange(mouth.depth, 0.f, 1.f, "mouth.depth");
}

string SingleMountainSettings::GetName() const {
    return "";
}


static const string kCount = "count";

void MountainSettings::Deserialize(JsonObject config) {
    count = config[kCount];
    settings.resize(count);
    JsonArray mountain_settings = config[kSettings];
    if (count > mountain_settings.size()) {
        count = static_cast<int>(config[kSettings]);
    }
    for (int i = 0; i < count; ++i) {
        settings[i].Deserialize(mountain_settings[i]);
    }
}

JsonObject MountainSettings::Serialize() const {
    JsonObject config;
    config[kCount] = count;
    JsonArray config_array;
    config_array.reserve(count);
    for (const auto& setting : settings) {
        config_array.push_back(setting.Serialize());
    }
    config[kSettings] = config_array;
    return config;
}

void MountainSettings::Check() const {
    if (settings.size() < count) {
        throw LogicException(
            "Mountain settings count is less than mountain count.");
    }
    for (auto& mountain : settings) {
        mountain.Check();
    }
}

string MountainSettings::GetName() const {
    return kConfigMountain;
}

} // namespace prowogene
} // namespace modules
