#include "location.h"

namespace prowogene {
namespace modules {

using std::string;
using utils::JsonValue;
using utils::JsonObject;
using utils::RgbaPixel;

static const string kEnabled =       "enabled";
static const string kForestOctaves = "forest_octaves";
static const string kForestRatio =   "forest_ratio";
static const string kExportMap =     "export_map";
static const string kColors =        "colors";
static const string kColorsNone =     "none";
static const string kColorsForest =   "forest";
static const string kColorsGlade =    "glade";
static const string kColorsMountain = "mountain";
static const string kColorsRiver =    "river";
static const string kColorsBeach =    "beach";
static const string kColorsSea =      "sea";

void LocationSettings::Deserialize(JsonObject config) {
    enabled =        config[kEnabled];
    forest_octaves = config[kForestOctaves];
    forest_ratio =   config[kForestRatio];
    map_enable =     config[kExportMap];
    JsonObject json_colors = config[kColors];
    colors.none =     RgbaPixel(json_colors[kColorsNone].Str());
    colors.forest =   RgbaPixel(json_colors[kColorsForest].Str());
    colors.glade =    RgbaPixel(json_colors[kColorsGlade].Str());
    colors.mountain = RgbaPixel(json_colors[kColorsMountain].Str());
    colors.river =    RgbaPixel(json_colors[kColorsRiver].Str());
    colors.beach =    RgbaPixel(json_colors[kColorsBeach].Str());
    colors.sea =      RgbaPixel(json_colors[kColorsSea].Str());
}

JsonObject LocationSettings::Serialize() const {
    JsonObject config;
    config[kEnabled] =       enabled;
    config[kForestOctaves] = forest_octaves;
    config[kForestRatio] =   forest_ratio;
    config[kExportMap] =     map_enable;
    JsonObject json_colors;
    json_colors[kColorsNone] =     colors.none.ToString();
    json_colors[kColorsForest] =   colors.forest.ToString();
    json_colors[kColorsGlade] =    colors.glade.ToString();
    json_colors[kColorsMountain] = colors.mountain.ToString();
    json_colors[kColorsRiver] =    colors.river.ToString();
    json_colors[kColorsBeach] =    colors.beach.ToString();
    json_colors[kColorsSea] =      colors.sea.ToString();
    config[kColors] = json_colors;
    return config;
}

string LocationSettings::GetName() const {
    return kConfigLocation;
}

} // namespace modules
} // namespace prowogene
