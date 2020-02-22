#include "post_process.h"

namespace prowogene {
namespace modules {

using std::string;
using utils::JsonValue;
using utils::JsonObject;

static const string kEnabled =             "enabled";
static const string kHeightmap =           "heightmap";
static const string kHeightmapCropBottom = "crop_bottom";
static const string kHeightmapCropTop =    "crop_top";
static const string kHeightmapPower =      "power";
static const string kHeightmapInvert =     "invert";

void PostprocessSettings::Deserialize(JsonObject config) {
    enabled = config[kEnabled];
    JsonObject config_hm = config[kHeightmap];
    heightmap.crop_bottom = config_hm[kHeightmapCropBottom];
    heightmap.crop_top =    config_hm[kHeightmapCropTop];
    heightmap.power =       config_hm[kHeightmapPower];
    heightmap.invert =      config_hm[kHeightmapInvert];
}

JsonObject PostprocessSettings::Serialize() const {
    JsonObject config;
    config[kEnabled] = enabled;
    JsonObject config_hm;
    config_hm[kHeightmapCropBottom] = heightmap.crop_bottom;
    config_hm[kHeightmapCropTop] =    heightmap.crop_top;
    config_hm[kHeightmapPower] =      heightmap.power;
    config_hm[kHeightmapInvert] =     heightmap.invert;
    config[kHeightmap] = config_hm;
    return config;
}

bool PostprocessSettings::IsCorrect() const {
    if (heightmap.crop_bottom < 0.0f || heightmap.crop_bottom > 1.0f ||
            heightmap.crop_top < 0.0f || heightmap.crop_top > 1.0f ||
            heightmap.crop_bottom > heightmap.crop_top) {
        return false;
    }
    return true;
}

string PostprocessSettings::GetName() const {
    return kConfigPostProcess;
}

} // namespace modules
} // namespace prowogene
