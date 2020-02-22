#ifndef PROWOGENE_CORE_MODULES_SYSTEM_SETTINGS_H_
#define PROWOGENE_CORE_MODULES_SYSTEM_SETTINGS_H_

#include "settings_interface.h"

namespace prowogene {
namespace modules {

/** System settings config name. */
static const std::string kConfigSystem =  "system";

/** @brief  System settings. */
struct SystemSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;
    /** Detects when thread_count is set to 0 and set it to real thread count
    on device and set their count. That function is called inside Deserialize
    method, but when filling this settings manually, That method must be
    called manually too. */
    void SetRealThreadCount();


    /** Count of binary search steps. */
    int search_depth = 10;
    /** Maximal thread count for processing. 0 means automatically detect it
    from device and use as much as possible. */
    int thread_count = 0;
    /** File extensions. */
    struct {
        /** File extensions for images. */
        std::string image = "bmp";
        /** File extensions for models. */
        std::string model = "obj";
    } extensions;
};

} // namespace modules
} // namespace prowogene

#endif // PROWOGENE_CORE_MODULES_SYSTEM_SETTINGS_H_
