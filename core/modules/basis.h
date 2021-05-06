#ifndef PROWOGENE_CORE_MODULES_BASIS_H_
#define PROWOGENE_CORE_MODULES_BASIS_H_

#include "general_settings.h"
#include "module_interface.h"
#include "system_settings.h"
#include "types.h"
#include "utils/array2d.h"

namespace prowogene {
namespace modules {

/** Basis settings config name. */
static const std::string kConfigBasis = "basis";

/** @brief Basis module settings. */
struct BasisSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::IsCorrect */
    bool IsCorrect() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Basis surface height relative to total height. [0.0, 1.0]. */
    float      height = 0.5f;
    /** Basis surface generation algoritm. Any. */
    Surface    surface = Surface::DiamondSquare;
    /** Nosie details size. Less values means bigger details. [1, ...]. */
    int        periodicity = 1;
    /** Nosie distortion type. Any. */
    Distortion distortion = Distortion::Quadric;
    /** Align of key_point. Any. */
    Align      align = Align::Center;
    /** Key point of surface that will be aligned. Any. */
    KeyPoint   key_point = KeyPoint::Default;
};

/** @brief Module for basis surface creation.

Creates basis surface without any high quality details or structures. */
class BasisModule : public IModule {
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
        /** Basis settings. */
        BasisSettings   basis;
        /** General settings. */
        GeneralSettings general;
        /** System settings. */
        SystemSettings  system;
    } settings_;
};

} // namespace modules
} // namespace prowogene

#endif  // PROWOGENE_CORE_MODULES_BASIS_H_
