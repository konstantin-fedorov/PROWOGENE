#include "filename_settings.h"

namespace prowogene {
namespace modules {

using std::string;
using utils::JsonObject;

static const string kPrefix =    "prefix";
static const string kSeparator = "coord_separator";
static const string kPostfix =   "postfix";

void FilenameSettings::Deserialize(JsonObject config) {
    prefix =          config[kPrefix].Str();
    coord_separator = config[kSeparator].Str();
    postfix =         config[kPostfix].Str();
}

JsonObject FilenameSettings::Serialize() const {
    JsonObject config;
    config[kPrefix] =    prefix;
    config[kSeparator] = coord_separator;
    config[kPostfix] =   postfix;
    return config;
}

string FilenameSettings::GetName() const {
    return "";
}

FilenameSettings::FilenameSettings(const string& pref,
        const string& separator, const string& post) {
    prefix =          pref;
    coord_separator = separator;
    postfix =         post;
}

string FilenameSettings::Apply(int x, int y, string extention) const {
    if (extention.size()) {
        extention = "." + extention;
    }
    return prefix + std::to_string(x) + coord_separator + std::to_string(y) +
           postfix + extention;
}

} // namespace modules
} // namespace prowogene
