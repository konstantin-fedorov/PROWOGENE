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

void PostprocessSettings::Check() const {
    CheckInRange(heightmap.crop_bottom, 0.f, 1.f, "heightmap.crop_bottom");
    CheckInRange(heightmap.crop_top,    0.f, 1.f, "heightmap.crop_top");
    CheckCondition(heightmap.crop_bottom <= heightmap.crop_top,
                   "heightmap.crop_bottom is more than heightmap.crop_top");
}

string PostprocessSettings::GetName() const {
    return kConfigPostProcess;
}

} // namespace modules
} // namespace prowogene
