#ifndef PROWOGENE_CORE_MODULES_FILENAME_SETTINGS_H_
#define PROWOGENE_CORE_MODULES_FILENAME_SETTINGS_H_

#include "settings_interface.h"

namespace prowogene {
namespace modules {

/** @brief Construction filenames info that contain 2D coordinates inside. */
struct FilenameSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Constructor. */
    FilenameSettings() {};

    /** Constructor. */
    FilenameSettings(const std::string& prefix,
                     const std::string& separator,
                     const std::string& postfix);

    /** Create filename according to stored data.
    Example:
        prefix =          "tex_"
        coord_separator = "x"
        postfix =         "_chunk"
    When call Apply(3, 5, ".bmp") "tex_3x5_chunk.bmp" will be returned. */
    virtual std::string Apply(int x, int y, std::string ext = "") const;


    /** Text before coordinates. */
    std::string prefix = "_";
    /** Text between coordinates. */
    std::string coord_separator = "x";
    /** Text after coordinates, but before extension. */
    std::string postfix = "";
};

} // namespace modules
} // namespace prowogene

#endif // PROWOGENE_CORE_MODULES_FILENAME_SETTINGS_H_
