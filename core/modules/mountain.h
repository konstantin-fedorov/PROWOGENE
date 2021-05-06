#ifndef PROWOGENE_CORE_MODULES_MOUNTAIN_H_
#define PROWOGENE_CORE_MODULES_MOUNTAIN_H_

#include "general_settings.h"
#include "modules/basis.h"
#include "modules/location.h"
#include "types.h"

namespace prowogene {
namespace modules {

/** Mountain settings config name. */
static const std::string kConfigMountain = "mountain";

/** @brief Single mountain preferences. */
struct SingleMountainSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::IsCorrect */
    bool IsCorrect() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Mountain size. [2, ...), power of 2. */
    int      size = 128;
    /** Height of mountain. [0.0, 1.0] */
    float    height = 1.0f;
    /** Count of gradient noises for mountain creation. [1, ...) */
    int      noises_count = 6;
    /** Type of conus shape. */
    Gradient gradient = Gradient::Linear;
    /** Type of completed surface shape transformation. */
    Gradient hillside = Gradient::Linear;
    /** Align of mountain peak on world. */
    Align    align = Align::Random;
    /** Voolcano mouth. */
    struct {
        /** Add voolcano mouth or not. */
        bool  enabled = false;
        /** Diameter of mouth relatively to mountain size. */
        float width = 0.5f;
        /** Voolcano mouth depth. */
        float depth = 0.5f;
    } mouth;
};

/** @brief Mountain module settings. */
struct MountainSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::IsCorrect */
    bool IsCorrect() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Count of mountains to place. Only first ones will be placed if it's
    smaller than single mountain settings count. */
    int  count = 0;
    /** Settings for each mountain. */
    std::vector<SingleMountainSettings> settings = { };
};


/** @brief Module for mountain creation.

Creates mountains and places them on heightmap. Also marks their locations. */
class MountainModule : public IModule {
 public:
    /** @copydoc IModule::Process */
    bool Process() override;
    /** @copydoc IModule::GetNeededSettings */
    std::list<std::string> GetNeededSettings() const override;
    /** @copydoc IModule::ApplySettings */
    void ApplySettings(ISettings* settings) override;
    /** @copydoc IModule::GetName */
    std::string GetName() const override;

 protected:
    /** Create mountain.
    @param [out] mountain    - Height map of mountain.
    @param [in] settings     - Mountain settings.
    @param [in] seed         - Random number generation seed.
    @param [in] thread_count - Maximal count of threads for processing. */
    static void Mountain(utils::Array2D<float>& mountain,
                         const SingleMountainSettings& settings,
                         int seed,
                         int thread_count);

    /** Create voolcano mouth for mountain.
    @param [in, out] mountain - Height map of mountain.
    @param [in] settings      - Mountain settings.
    @param [in] seed          - Random number generation seed.
    @param [in] thread_count  - Maximal count of threads for processing. */
    static void AddMouth(utils::Array2D<float>& mountain,
                         const SingleMountainSettings& settings,
                         int seed,
                         int thread_count);

    /** Marks all mountains on location map.
    @param [in] ridge - Combined height map for all mountains. */
    virtual void MarkMountains(const utils::Array2D<float>& ridge);

 public:
    /** Height map from data storage. */
    utils::Array2D<float>*    height_map_ = nullptr;
    /** Location map from data storage. */
    utils::Array2D<Location>* location_map_ = nullptr;
    /** Mountain mask from data storage. */
    utils::Array2D<float>*    mountain_mask_ = nullptr;

 private:
    /** Settings for module. */
    struct {
        /** Basis settings. */
        BasisSettings   basis;
        /** General settings. */
        GeneralSettings general;
        /** Location settings. */
        LocationSettings location;
        /** Mountain settings. */
        MountainSettings mountain;
        /** System settings. */
        SystemSettings   system;
    } settings_;
};

}  // namespace modules
}  // namespace prowogene

#endif  // PROWOGENE_CORE_MODULES_MOUNTAIN_H_
