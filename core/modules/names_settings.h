#ifndef PROWOGENE_CORE_MODULES_NAMES_SETTINGS_H_
#define PROWOGENE_CORE_MODULES_NAMES_SETTINGS_H_

#include "filename_settings.h"

namespace prowogene {
namespace modules {

/** Names settings config name. */
static const std::string kConfigNames = "names";

/** @brief Setting that specify filenames of generatior output.
All filenames are set without extensions. */
struct NamesSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Height map image filename. */
    std::string heightmap = "height_map";
    /** Chunk's texture map image filename template. */
    FilenameSettings texture = FilenameSettings("tex_",    "x", "");
    /** Chunk's normal map image filename template. */
    FilenameSettings normal =  FilenameSettings("normal_", "x", "");
    /** Chunk's 3D model filename template. */
    FilenameSettings chunk =   FilenameSettings("chunk_",  "x", "");
    /** Minimap filenames. */
    struct {
        /** Minimap's texture map image filename. */
        std::string texture = "minimap_texture";
        /** Minimap's normal map image filename. */
        std::string normal =  "minimap_normal";
        /** Minimap's 3D model filename. */
        std::string model =   "minimap_model";
    } minimap;
    /** Location map image filename. */
    std::string location_map = "locations";
};

} // namespace modules
} // namespace prowogene

#endif // PROWOGENE_CORE_MODULES_NAMES_SETTINGS_H_
