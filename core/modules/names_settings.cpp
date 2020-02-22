#include "names_settings.h"

namespace prowogene {
namespace modules {

using std::string;
using utils::JsonObject;

static const string kHeightMap =      "heightmap";
static const string kTexture =        "texture";
static const string kNormal =         "normal";
static const string kChunk =          "chunk";
static const string kMinimap =        "minimap";
static const string kMinimapTexture = "texture";
static const string kMinimapModel =   "model";
static const string kMinimapNormal =  "normal";
static const string kLocationMap =    "location_map";
static const string kComplexMap =     "complex_map";

void NamesSettings::Deserialize(JsonObject config) {
    heightmap = config[kHeightMap].Str();
    texture.Deserialize(config[kTexture]);
    normal.Deserialize( config[kNormal]);
    chunk.Deserialize(  config[kChunk]);
    JsonObject sub_config = config[kMinimap];
    minimap.texture = sub_config[kMinimapTexture].Str();
    minimap.model =   sub_config[kMinimapModel].Str();
    minimap.normal =  sub_config[kMinimapNormal].Str();
    location_map = config[kLocationMap].Str();
}

JsonObject NamesSettings::Serialize() const {
    JsonObject config;
    config[kHeightMap] =   heightmap;
    config[kLocationMap] = location_map;
    config[kTexture] = texture.Serialize();
    config[kNormal] =  normal.Serialize();
    config[kChunk] =   chunk.Serialize();
    JsonObject json_minimap;
    json_minimap[kMinimapTexture] = minimap.texture;
    json_minimap[kMinimapModel] =   minimap.model;
    json_minimap[kMinimapNormal] =  minimap.normal;
    config[kMinimap] = json_minimap;
    return config;
}

string NamesSettings::GetName() const {
    return kConfigNames;
}

} // namespace modules
} // namespace prowogene
