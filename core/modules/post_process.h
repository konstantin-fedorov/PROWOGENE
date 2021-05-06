#ifndef PROWOGENE_CORE_MODULES_POST_PROCESS_H_
#define PROWOGENE_CORE_MODULES_POST_PROCESS_H_

#include "system_settings.h"
#include "module_interface.h"
#include "utils/array2d.h"

namespace prowogene {
namespace modules {

/** Post process settings config name. */
static const std::string kConfigPostProcess = "post_process";

/** @brief Post process module settings. */
struct PostprocessSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::IsCorrect */
    bool IsCorrect() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Enable post procesing or not. */
    bool enabled = false;
    /** Height map post process settings. */
    struct {
        /** Set all lower values to that level. */
        float crop_bottom = 0.0f;
        /** Set all higher values to that level. */
        float crop_top = 1.0f;
        /** Exponent for each height map value (after cropping). */
        float power = 1.0f;
        /** Invert height map. */
        bool  invert = false;
    } heightmap;
};


/** @brief Module for post processing height map.

Invert height map, crops it or exponentiate all values in it. */
class PostProcessModule : public IModule {
 public:
    /** @copydoc IModule::Process */
    bool Process() override;
    /** @copydoc IModule::GetNeededSettings */
    std::list<std::string> GetNeededSettings() const override;
    /** @copydoc IModule::ApplySettings */
    void ApplySettings(ISettings* settings) override;
    /** @copydoc IModule::GetName */
    std::string GetName() const override;

 public:
    /** Height map from data storage. */
    utils::Array2D<float>* height_map_ = nullptr;

 protected:
    /** Settings for module. */
    struct {
        /** Post process settings. */
        PostprocessSettings postprocess;
        /** System settings. */
        SystemSettings      system;
    } settings_;
};

} // namespace modules
} // namespace prowogene

#endif // PROWOGENE_CORE_MODULES_POST_PROCESS_H_
