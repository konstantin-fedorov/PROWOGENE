#include "texture.h"

namespace prowogene {
namespace modules {

using std::string;
using std::vector;
using utils::JsonValue;
using utils::JsonObject;
using utils::JsonArray;
using utils::RgbaPixel;
using GradientPoint = std::pair<RgbaPixel, float>;

static const string kHeightmapEnabled =     "heightmap_enabled";
static const string kChunksEnabled =        "chunks_enabled";
static const string kMinimap =              "minimap";
static const string kMinimapEnabled =       "enabled";
static const string kMinimapTileSize =      "tile_size";
static const string kMinimapClearEdges =    "clear_edges";
static const string kNormals =              "normals";
static const string kNormalsEnabled =       "enabled";
static const string kNormalsInvert =        "invert";
static const string kNormalsStrength =      "strength";
static const string kSplatting =            "splatting";
static const string kSplattingDepth =       "depth";
static const string kSplattingBandwidth =   "bandwidth";
static const string kSplattingRandomness =  "randomness";
static const string kGradient =             "gradient";
static const string kGradientEnabled =      "enabled";
static const string kGradientOpacity =      "opacity";
static const string kGradientPoints =       "points";
static const string kGradientOnlyMinimap =  "only_minimap";
static const string kShadow =               "shadow";
static const string kShadowEnabled =        "enabled";
static const string kShadowStrength =       "strength";
static const string kShadowAngle =          "angle";
static const string kTargetBitDepth =       "target_bitdepth";
static const string kImages =               "images";
static const string kImagesBases =          "bases";
static const string kImagesBasesMountain =  "mountain";
static const string kImagesBasesBasis =     "basis";
static const string kImagesBasesSea =       "sea";
static const string kImagesBasesRiver =     "river";
static const string kImagesBasesBeach =     "beach";
static const string kImagesDecals =         "decals";
static const string kImagesDecalsEnabled =  "enabled";
static const string kImagesDecalsChance =   "chance";
static const string kImagesDecalsMountain = "mountain";
static const string kImagesDecalsBasis =    "basis";
static const string kImagesDecalsSea =      "sea";
static const string kImagesDecalsRiver =    "river";
static const string kImagesDecalsBeach =    "beach";

void TextureSettings::Deserialize(JsonObject config) {
    JsonObject sub_config;
    height_map_enabled = config[kHeightmapEnabled];
    chunks_enabled =     config[kChunksEnabled];
    target_bitdepth =    config[kTargetBitDepth];
    sub_config = config[kMinimap];
    minimap.enabled =     sub_config[kMinimapEnabled];
    minimap.tile_size =   sub_config[kMinimapTileSize];
    minimap.clear_edges = sub_config[kMinimapClearEdges];
    sub_config = config[kNormals];
    normals.enabled =  sub_config[kNormalsEnabled];
    normals.strength = sub_config[kNormalsStrength];
    normals.invert =   sub_config[kNormalsInvert];
    sub_config = config[kSplatting];
    splatting.depth =      sub_config[kSplattingDepth];
    splatting.bandwidth =  sub_config[kSplattingBandwidth];
    splatting.randomness = sub_config[kSplattingRandomness];
    sub_config = config[kGradient];
    gradient.enabled = sub_config[kGradientEnabled];
    gradient.opacity = sub_config[kGradientOpacity];
    gradient.points = GetGradientPoints(sub_config[kGradientPoints]);
    gradient.only_minimap = sub_config[kGradientOnlyMinimap];
    sub_config = config[kShadow];
    shadow.enabled =  sub_config[kShadowEnabled];
    shadow.strength = sub_config[kShadowStrength];
    shadow.angle =    sub_config[kShadowAngle];
    sub_config = config[kImages];
    JsonObject json_images = sub_config[kImagesBases];
    auto& bases = images.bases;
    bases.mountain = json_images[kImagesBasesMountain].Str();
    bases.basis =    json_images[kImagesBasesBasis].Str();
    bases.sea =      json_images[kImagesBasesSea].Str();
    bases.river =    json_images[kImagesBasesRiver].Str();
    bases.beach =    json_images[kImagesBasesBeach].Str();
    json_images = sub_config[kImagesDecals];
    auto& decals = images.decals;
    decals.enabled = json_images[kImagesDecalsEnabled];
    decals.chance =  json_images[kImagesDecalsChance];
    decals.mountain = GetDecals(json_images[kImagesDecalsMountain]);
    decals.basis =    GetDecals(json_images[kImagesDecalsBasis]);
    decals.sea =      GetDecals(json_images[kImagesDecalsSea]);
    decals.river =    GetDecals(json_images[kImagesDecalsRiver]);
    decals.beach =    GetDecals(json_images[kImagesDecalsBeach]);
}

JsonObject TextureSettings::Serialize() const {
    JsonObject config;
    config[kHeightmapEnabled] = height_map_enabled;
    config[kChunksEnabled] = chunks_enabled;
    config[kTargetBitDepth] = target_bitdepth;
    JsonObject json_minimap;
    json_minimap[kMinimapEnabled] =    minimap.enabled;
    json_minimap[kMinimapEnabled] =    minimap.tile_size;
    json_minimap[kMinimapClearEdges] = minimap.clear_edges;
    config[kMinimap] = json_minimap;
    JsonObject json_normals;
    json_normals[kNormalsEnabled] =  normals.enabled;
    json_normals[kNormalsStrength] = normals.strength;
    json_normals[kNormalsInvert] =   normals.invert;
    config[kNormals] = json_normals;
    JsonObject json_splatting;
    json_splatting[kSplattingDepth] =      splatting.depth;
    json_splatting[kSplattingBandwidth] =  splatting.bandwidth;
    json_splatting[kSplattingRandomness] = splatting.randomness;
    config[kSplatting] = json_splatting;
    JsonObject json_grad;
    json_grad[kGradientEnabled] = gradient.enabled;
    json_grad[kGradientOpacity] = gradient.opacity;
    json_grad[kGradientPoints] = SetGradientPoints(gradient.points);
    json_grad[kGradientOnlyMinimap] = gradient.only_minimap;
    config[kGradient] = json_grad;
    JsonObject json_shadow;
    json_shadow[kShadowEnabled] =  shadow.enabled;
    json_shadow[kShadowStrength] = shadow.strength;
    json_shadow[kShadowAngle] =    shadow.angle;
    config[kShadow] = json_shadow;
    JsonObject json_images;
    JsonObject json_bases;
    json_bases[kImagesBasesMountain] = images.bases.mountain;
    json_bases[kImagesBasesBasis] =    images.bases.basis;
    json_bases[kImagesBasesSea] =      images.bases.sea;
    json_bases[kImagesBasesRiver] =    images.bases.river;
    json_bases[kImagesBasesBeach] =    images.bases.beach;
    json_images[kImagesBases] = json_bases;
    JsonObject json_decals;
    const auto& decals = images.decals;
    json_decals[kImagesDecalsEnabled] =  decals.enabled;
    json_decals[kImagesDecalsChance] =   decals.chance;
    json_decals[kImagesDecalsMountain] = SetDecals(decals.mountain);
    json_decals[kImagesDecalsBasis] =    SetDecals(decals.basis);
    json_decals[kImagesDecalsSea] =      SetDecals(decals.sea);
    json_decals[kImagesDecalsRiver] =    SetDecals(decals.river);
    json_decals[kImagesDecalsBeach] =    SetDecals(decals.beach);
    json_images[kImagesDecals] = json_decals;
    config[kImages] = json_images;
    return config;
}

void TextureSettings::Check() const {
    CheckInRange(normals.strength,     0.f, 1.f, "normals.strength");
    CheckInRange(splatting.depth,      0.f, 1.f, "splatting.depth");
    CheckInRange(splatting.bandwidth,  0.f, 1.f, "splatting.bandwidth");
    CheckInRange(splatting.randomness, 0.f, 1.f, "splatting.randomness");
    CheckInRange(gradient.opacity,     0.f, 1.f, "gradient.opacity");
    CheckInRange(shadow.strength,      0.f, 1.f, "shadow.strength");
    CheckInRange(images.decals.chance, 0.f, 1.f, "normals.strength");
    if (!minimap.enabled)
        return;
    CheckCondition(minimap.tile_size > 0, "minimap.tile_size is less than 1");
    CheckCondition(!(minimap.tile_size & (minimap.tile_size - 1)),
                   "minimap.tile_size isn't a power of 2");
}

string TextureSettings::GetName() const {
    return kConfigTexture;
}

GradientPoints TextureSettings::GetGradientPoints(const JsonArray& arr) {
    GradientPoints out;
    out.reserve(arr.size());
    for (const auto& elem : arr) {
        const string str = elem;
        const size_t space = str.find(' ');
        if (space == string::npos) {
            continue;
        }
        const string color = str.substr(0, space);
        RgbaPixel pixel(color);
        string float_val = str.substr(space + 1, str.size() - space);

        GradientPoint point;
        point.first = pixel;
        point.second = static_cast<float>(atof(float_val.c_str()));
        out.push_back(point);
    }
    return out;
}

JsonArray TextureSettings::SetGradientPoints(const GradientPoints& arr) {
    JsonArray out;
    out.reserve(arr.size());
    for (const auto& elem : arr) {
        string str = elem.first.ToString() + " " + std::to_string(elem.second);
        out.push_back(str);
    }
    return out;
}

vector<string> TextureSettings::GetDecals(const JsonArray& arr) {
    vector<string> out;
    out.reserve(arr.size());
    for (const auto& elem : arr) {
        out.push_back(elem);
    }
    return out;
}

JsonArray TextureSettings::SetDecals(const vector<string>& arr) {
    JsonArray out;
    out.reserve(arr.size());
    for (const auto& elem : arr) {
        out.push_back(elem);
    }
    return out;
}

} // namespace modules
} // namespace prowogene
