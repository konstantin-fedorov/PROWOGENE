#ifndef PROWOGENE_CORE_MODULE_INTERFACE_H_
#define PROWOGENE_CORE_MODULE_INTERFACE_H_

#include <list>
#include <string>

#include "settings_interface.h"

namespace prowogene {

/** @brief Module interface which describes separate step of generation.

Allows to integrate module to generator. Also contains a few usefull
methods for work with settings and data from storage. */
class IModule {
 public:
    /** Initialize own module values. */
    virtual void Init() {};

    /** Deinitialize own module values. */
    virtual void Deinit() {};

    /** Launch generation step
    @return @c true is step completed successfully, @c false when errors
            occurs during processing. */
    virtual void Process() = 0;

    /** Get needed settings keys that need to be attached.
    @return List of settings keys. */
    virtual std::list<std::string> GetNeededSettings() const = 0;

    /** Fill single settings entity if settings with it's key is required. */
    virtual void ApplySettings(ISettings* settings) { };

    /** Get module's name.
    @return Name of module. */
    virtual std::string GetName() const = 0;

 protected:
    /** Copy settings from source to destination if source's key is equal
    to given.
    @param [in] src  - Source of copying settings entity.
    @param [out] dst - Destination of copying settings entity. Must be derived
                       from ISettings. */
    template<typename T>
    static void CopySettings(const ISettings* src, T& dst) {
        if (src) {
            if (src->GetName() == dst.GetName()) {
                T* casted = (T*)(src);
                dst = *casted;
            }
        }
    }
};

} // namespace prowogene

#endif // PROWOGENE_CORE_MODULE_INTERFACE_H_
