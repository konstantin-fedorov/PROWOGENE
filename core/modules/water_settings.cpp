#include "water.h"

namespace prowogene {
namespace modules {

using std::string;
using utils::JsonValue;
using utils::JsonObject;

static const string kSea = "sea";
static const string kSeaEnabled =   "enabled";
static const string kSeaErosion =   "erosion";
static const string kSeaRatio =     "ratio";
static const string kBeach =        "beach";
static const string kBeachEnabled = "enabled";
static const string kBeachRatio =   "ratio";

void WaterSettings::Deserialize(JsonObject config) {
    JsonObject sea_json = config[kSea];
    sea.enabled = sea_json[kSeaEnabled];
    sea.erosion = sea_json[kSeaErosion];
    sea.ratio =   sea_json[kSeaRatio];
    JsonObject beach_json = config[kBeach];
    beach.enabled = sea_json[kBeachEnabled];
    beach.ratio =   sea_json[kBeachRatio];
}

JsonObject WaterSettings::Serialize() const {
    JsonObject config;
    JsonObject sea_json;
    sea_json[kSeaEnabled] = sea.enabled;
    sea_json[kSeaErosion] = sea.erosion;
    sea_json[kSeaRatio] = sea.ratio;
    config[kSea] = sea_json;
    JsonObject beach_json;
    beach_json[kBeachEnabled] = beach.enabled;
    beach_json[kBeachRatio] = beach.ratio;
    config[kBeach] = beach_json;
    return config;
}

void WaterSettings::Check() const {
    CheckInRange(sea.ratio,               0.f, 1.f, "sea.ratio");
    CheckInRange(beach.ratio,             0.f, 1.f, "beach.ratio");
    CheckInRange(beach.ratio + sea.ratio, 0.f, 1.f, "beach.ratio + sea.ratio");
}

string WaterSettings::GetName() const {
    return kConfigWater;
}

} // namescpace modules
} // namescpace prowogene
