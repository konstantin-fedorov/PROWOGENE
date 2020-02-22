#include "model.h"

namespace prowogene {
namespace modules {

using std::string;
using utils::JsonValue;
using utils::JsonObject;

static const string kChunksEnabled =    "chunks_enabled";
static const string kMinimapEnabled =   "complex_enabled";
static const string kMaterialsEnabled = "materials_enabled";
static const string kUVEnabled =        "uv_enabled";
static const string kNormalsEnabled =   "normals_enabled";
static const string kEdgeSize =         "edge_size";
static const string kMapHeight =        "map_height";
static const string kCoordFormat =      "coord_format";

void ModelSettings::Deserialize(JsonObject config) {
    chunks_enabled =    config[kChunksEnabled];
    uv_enabled =        config[kUVEnabled];
    normals_enabled =   config[kNormalsEnabled];
    materials_enabled = config[kMaterialsEnabled];
    complex_enabled =   config[kMinimapEnabled];
    edge_size =         config[kEdgeSize];
    map_height =        config[kMapHeight];
    coord_format =      config[kCoordFormat].Str();
}

JsonObject ModelSettings::Serialize() const {
    JsonObject config;
    config[kChunksEnabled] =    chunks_enabled;
    config[kUVEnabled] =        uv_enabled;
    config[kNormalsEnabled] =   normals_enabled;
    config[kMaterialsEnabled] = materials_enabled;
    config[kMinimapEnabled] =   complex_enabled;
    config[kEdgeSize] =         edge_size;
    config[kMapHeight] =        map_height;
    config[kCoordFormat] =      coord_format;
    return config;
}

bool ModelSettings::IsCorrect() const {
    return (map_height > 0.0f - kEps);
}

string ModelSettings::GetName() const {
    return kConfigModel;
}

} // namespace modules
} // namespace prowogene
