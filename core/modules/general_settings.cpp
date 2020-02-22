#include "general_settings.h"

namespace prowogene {
namespace modules {

using std::string;
using utils::JsonObject;

static const string kSeed =      "seed";
static const string kSize =      "size";
static const string kChunkSize = "chunk_size";

void GeneralSettings::Deserialize(JsonObject config) {
    seed =       config[kSeed];
    size =       config[kSize];
    chunk_size = config[kChunkSize];
}

JsonObject GeneralSettings::Serialize() const {
    JsonObject config;
    config[kSeed] =      seed;
    config[kSize] =      size;
    config[kChunkSize] = chunk_size;
    return config;
}

bool GeneralSettings::IsCorrect() const {
    if ((size & (size - 1)) ||
            (chunk_size & (chunk_size - 1)) ||
            size < chunk_size) {
        return false;
    }
    return true;
}

string GeneralSettings::GetName() const {
    return kConfigGeneral;
}

} // namespace modules
} // namespace prowogene
