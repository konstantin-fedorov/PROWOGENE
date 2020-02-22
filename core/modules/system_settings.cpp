#include "system_settings.h"

#include <thread>

namespace prowogene {
namespace modules {

using std::string;
using std::thread;
using utils::JsonObject;

static const string kSearchDepth =     "search_depth";
static const string kThreadCount =     "thread_count";
static const string kExtensions =      "extensions";
static const string kExtensionsImage = "image";
static const string kExtensionsModel = "model";

void SystemSettings::Deserialize(JsonObject config) {
    search_depth = config[kSearchDepth];
    thread_count = config[kThreadCount];
    SetRealThreadCount();
    JsonObject json_ext = config[kExtensions];
    extensions.image = json_ext[kExtensionsImage].Str();
    extensions.model = json_ext[kExtensionsModel].Str();
}

JsonObject SystemSettings::Serialize() const {
    JsonObject config;
    config[kSearchDepth] = search_depth;
    config[kThreadCount] = thread_count;
    JsonObject json_ext;
    json_ext[kExtensionsImage] = extensions.image;
    json_ext[kExtensionsModel] = extensions.model;
    config[kExtensions] = json_ext;
    return config;
}

string SystemSettings::GetName() const {
    return kConfigSystem;
}

void SystemSettings::SetRealThreadCount() {
    if (thread_count > 0) {
        return;
    }
    thread_count = static_cast<int>(thread::hardware_concurrency());
    if (!thread_count) {
        thread_count = 1;
    }
}

} // namespace modules
} // namespace prowogene
