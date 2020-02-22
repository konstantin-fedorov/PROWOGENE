#include "river.h"

namespace prowogene {
namespace modules {

using std::string;
using utils::JsonValue;
using utils::JsonObject;
using utils::JsonArray;

static const string kSettings =          "settings";
static const string kMaxLengthInChunks = "max_length_in_chunks";
static const string kMaxHeadHeight =     "max_head_height";
static const string kDistortion =        "distortion";
static const string kChannel =           "channel";
static const string kChannelWidth =      "width";
static const string kChannelDepth =      "depth";

void SingleRiverSettings::Deserialize(JsonObject config) {
    max_length_in_chunks = config[kMaxLengthInChunks];
    max_head_height =      config[kMaxHeadHeight];
    distortion =           config[kDistortion];
    JsonObject channel_config = config[kChannel];
    channel.width =     channel_config[kChannelWidth];
    channel.depth =     channel_config[kChannelDepth];
}

JsonObject SingleRiverSettings::Serialize() const {
    JsonObject config;
    config[kMaxLengthInChunks] = max_length_in_chunks;
    config[kMaxHeadHeight] =     max_head_height;
    config[kDistortion] =        distortion;
    JsonObject channel_config;
    channel_config[kChannelWidth] =    channel.width;
    channel_config[kChannelDepth] =    channel.depth;
    config[kChannel] = channel_config;
    return config;
}

bool SingleRiverSettings::IsCorrect() const {
    if (max_head_height < 0.0f || max_head_height > 1.0f ||
            distortion < 0.0f || distortion > 1.0f ||
            channel.depth < 0.0f || channel.depth > 1.0f ||
            channel.width < 1) {
        return false;
    }
    return true;
}

string SingleRiverSettings::GetName() const {
    return "";
}


static const string kCount =        "count";
static const string kSmoothRadius = "smooth_radius";

void RiverSettings::Deserialize(JsonObject config) {
    count = config[kCount];
    smooth_radius = config[kSmoothRadius];
    settings.resize(count);
    JsonArray river_settings = config[kSettings];
    for (int i = 0; i < count; ++i) {
        settings[i].Deserialize(river_settings[i]);
    }
}

JsonObject RiverSettings::Serialize() const {
    JsonObject config;
    config[kCount] = count;
    config[kSmoothRadius] = smooth_radius;
    JsonArray rivers_array;
    for (int i = 0; i < count; ++i) {
        rivers_array.push_back(settings[i].Serialize());
    }
    return config;
}

bool RiverSettings::IsCorrect() const {
    if (settings.size() < count || smooth_radius < 0) {
        return false;
    }
    for (auto& river : settings) {
        if (!river.IsCorrect()) {
            return false;
        }
    }
    return true;
}

string RiverSettings::GetName() const {
    return kConfigRiver;
}

} // namescpace modules
} // namescpace prowogene
