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
    gradient =     TC::ToGradient(config[kGradient]);
    hillside =     TC::ToGradient(config[kHillside]);
    align =        TC::ToAlign(config[kAlign]);
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

bool SingleMountainSettings::IsCorrect() const {
    if (size < 2 || (size & (size - 1)) ||
            height < 0.0f || height > 1.0f ||
            noises_count < 1 ||
            mouth.width < 0.0f || mouth.width > 1.0f ||
            mouth.depth < 0.0f || mouth.depth > 1.0f) {
        return false;
    }
    return true;
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

bool MountainSettings::IsCorrect() const {
    if (settings.size() < count) {
        return false;
    }
    for (auto& mountain : settings) {
        if (!mountain.IsCorrect()) {
            return false;
        }
    }
    return true;
}

string MountainSettings::GetName() const {
    return kConfigMountain;
}

} // namespace prowogene
} // namespace modules
