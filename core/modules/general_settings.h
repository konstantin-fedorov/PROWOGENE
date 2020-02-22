#ifndef PROWOGENE_CORE_MODULES_GENERAL_SETTINGS_H_
#define PROWOGENE_CORE_MODULES_GENERAL_SETTINGS_H_

#include "settings_interface.h"

namespace prowogene {
namespace modules {

/** General settings config name. */
static const std::string kConfigGeneral = "general";

/** @brief General world settings. */
struct GeneralSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::IsCorrect */
    bool IsCorrect() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Random number generator seed. */
    int seed = 123;
    /** World discretization step. [2, ...), power of 2. */
    int size = 256;
    /** Chunks (pieces of world) discretization step. [2, size], power of 2.*/
    int chunk_size = 16;
};

} // namespace modules
} // namespace prowogene

#endif // PROWOGENE_CORE_MODULES_GENERAL_SETTINGS_H_
