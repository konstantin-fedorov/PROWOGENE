#ifndef PROWOGENE_CORE_MODULES_LOCATION_H_
#define PROWOGENE_CORE_MODULES_LOCATION_H_

#include "general_settings.h"
#include "names_settings.h"
#include "system_settings.h"
#include "modules/basis.h"
#include "modules/post_process.h"
#include "utils/image_io.h"

namespace prowogene {
namespace modules {

/** Location settings config name. */
static const std::string kConfigLocation = "location";

/** @brief Location module settings. */
struct LocationSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Location marking enabled. Any. */
    bool  enabled = false;
    /** Size of forest spots. Less values cause bigger spots. [1, map size] */
    int   forest_octaves = 1;
    /** Part of basis biome that will become forest. Other part will be glade.
    [0.0, 1.0] */
    float forest_ratio = 0.5f;
    /** Save locations map to image. Any. */
    bool  map_enable = false;
    /** Colors for location map image. */
    struct {
        /** Undefined location color. */
        utils::RgbaPixel none =     utils::RgbaPixel("#FF0000");
        /** Forest location color. */
        utils::RgbaPixel forest =   utils::RgbaPixel("#008800");
        /** Glade location color. */
        utils::RgbaPixel glade =    utils::RgbaPixel("#00FF00");
        /** Mountain location color. */
        utils::RgbaPixel mountain = utils::RgbaPixel("#888888");
        /** River location color. */
        utils::RgbaPixel river =    utils::RgbaPixel("#0000FF");
        /** Beach location color. */
        utils::RgbaPixel beach =    utils::RgbaPixel("#FFFF00");
        /** Sea location color. */
        utils::RgbaPixel sea =      utils::RgbaPixel("#000088");
    } colors;
};


/** @brief Module for marking different locations.

Marks locations according to height map, marks forest, glade add rivers. */
class LocationModule : public IModule {
 public:
    /** @copydoc IModule::Process */
    void Process() override;
    /** @copydoc IModule::GetNeededSettings */
    std::list<std::string> GetNeededSettings() const override;
    /** @copydoc IModule::ApplySettings */
    void ApplySettings(ISettings* settings) override;
    /** @copydoc IModule::GetName */
    std::string GetName() const override;

 protected:
    /** Save location map as image
    @param [in] filename - Image filename. */
    virtual void SaveMap(const std::string& filename);

    /** Mark locations according to height. */
    virtual void CreateBasedOnHeight();

    /** Mark river locations. */
    virtual void CreateRiverLocations();

    /** Divide basis biome to forest and glage locations. */
    virtual void DivideForestAndGlade();

    /** Get height where ratio of elements with height lower to
    all elements is equal to specified.
    @param [in] table - Elements for height detection.
    @param [in] ratio - Specified ratio of lower to all elements.
    @return Height that divides elements in specified ratio. */
    virtual float GetForestLevel(const utils::Array2D<float>& table,
                                 float ratio);

 public:
    /** Height map from data storage. */
    utils::Array2D<float>*    height_map_ = nullptr;
    /** River mask from data storage. */
    utils::Array2D<float>*    river_mask_ = nullptr;
    /** Sea level from data storage. */
    float*                    sea_level_ = nullptr;
    /** Beach level from data storage. */
    float*                    beach_level_ = nullptr;
    /** Location map from data storage. */
    utils::Array2D<Location>* location_map_ = nullptr;
    /** Image input/output worker from data storage. */
    utils::ImageIO*           image_io_ = nullptr;

 protected:
    /** Settings for module. */
    struct {
        /** Basis settings. */
        BasisSettings    basis;
        /** General settings. */
        GeneralSettings  general;
        /** Location settings. */
        LocationSettings location;
        /** Names settings. */
        NamesSettings    names;
        /** System settings. */
        SystemSettings   system;
    } settings_;
};

} // namespace modules
} // namespace prowogene

#endif // PROWOGENE_CORE_MODULES_LOCATION_H_
