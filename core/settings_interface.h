#ifndef PROWOGENE_CORE_SETTINGS_INTERFACE_H_
#define PROWOGENE_CORE_SETTINGS_INTERFACE_H_

#include <stdexcept>

#include "utils/json.h"

namespace prowogene {

/** @brief Excteption that will be thrown if any troubles are detected when
processing settings or module execution.

Other exceptions like (std::bad_alloc) will not be modified and will be
presented as-is. */
class LogicException : public std::logic_error {
public:
    LogicException(const std::string& msg) : std::logic_error(msg) { }
};


/** @brief Settings interface which describes preferences storage.

Allows to integrate settings to generator. Can be filled from and saved to
JSON file. All module's settings must be inside root JSON object, placed
in separate JSON object and named by their name. */
struct ISettings {
    /** Fill all values from config. If there are missings, settings will be
    set to default type value (not default setting!).
    @param [in] config - JSON object with settings. */
    virtual void Deserialize(utils::JsonObject config) = 0;
    
    /** Save all values to JSON config.
    @return JSON object with settings. */
    virtual utils::JsonObject Serialize() const = 0;

    /** Get setting's name that must be unique. That key is name of JSON
    object and also the key that used for settings connection with modules.
    @return Text name. */
    virtual std::string GetName() const = 0;

    /** Check settings for correctness.
    @return @c true if all settings are filled correctly, @c false otherwise. */
    virtual void Check() const { }

 protected:
    static void CheckCondition(bool cond, const std::string& text_cond_fail) {
        if (!cond) {
            throw LogicException(text_cond_fail);
        }
    }

    template <typename T>
    static void CheckInRange(T val, T min, T max, const std::string& var_name) {
        if ((val < min) || (val > max)) {
            std::string msg = "'" + var_name + "' must be in range [" +
                              std::to_string(min) + ", " + std::to_string(max) +
                              "]. Presented: " + std::to_string(val);
            throw LogicException(msg);
        }
    }
};

} // namespace prowogene

#endif // PROWOGENE_CORE_SETTINGS_INTERFACE_H_
