#ifndef PROWOGENE_CORE_MODULES_RIVER_H_
#define PROWOGENE_CORE_MODULES_RIVER_H_

#include "general_settings.h"
#include "system_settings.h"
#include "module_interface.h"
#include "utils/array2d.h"
#include "utils/random.h"
#include "types.h"

namespace prowogene {
namespace modules {

/** River settings config name. */
static const std::string kConfigRiver = "river";

/** @brief Single river settings. */
struct SingleRiverSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::IsCorrect */
    bool IsCorrect() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Maximal rivers length in chunks. */
    int   max_length_in_chunks = 3;
    /** Maximal height where river can start. [0.0, 1.0]*/
    float max_head_height      = 1.0f;
    /** Maximal curvature of river. When set to 0, river will be straight
    line. [0.0, 1.0] */
    float distortion           = 0.25f;
    /** Channel settings. */
    struct {
        /** Channel width in points. [1, 255] */
        int   width     = 6;
        /** Channel depth. [0.0, 1.0] */
        float depth     = 0.05f;
    } channel;
};


/** @brief River module settings. */
struct RiverSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::IsCorrect */
    bool IsCorrect() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Count of rivers to place. Only first ones will be placed if it's
    smaller than single river settings count. */
    int count = 0;
    /** Smooth radius for height map after all rivers creation. */
    int smooth_radius = 0;
    /** Settings for each river. */
    std::vector<SingleRiverSettings> settings = { };
};


/** @brief Module for river creation.

Creates rivers and places them on heightmap. Also marks river locations. */
class RiverModule : public IModule {
 public:
    /** @copydoc IModule::Init */
    void Init() override;
    /** @copydoc IModule::Deinit */
    void Deinit() override;
    /** @copydoc IModule::Process */
    bool Process() override;
    /** @copydoc IModule::GetNeededSettings */
    std::list<std::string> GetNeededSettings() const override;
    /** @copydoc IModule::ApplySettings */
    void ApplySettings(ISettings* settings) override;
    /** @copydoc IModule::GetName */
    std::string GetName() const override;

 protected:
    /** Find min and max values for all chunks. */
    virtual void ScanChunks();

    /** Detect positions where river can end. */
    virtual void DetecLastPoints();

    /** Create river on height map.
    @param [in] river      - River's settings.
    @param [in] river_mask - Mask of distances to channel's center. */
    virtual void CreateRiver(const SingleRiverSettings& river,
                             utils::Array2D<uint8_t> river_mask);

    /** Create river on height map.
    @param [in] top        - River's top point.
    @param [in] bottom     - River's bottom point.
    @param [in] river      - River's settings.
    @param [in] river_mask - Mask of distances to channel's center. */
    virtual void MarkChannel(const Point<float> &top,
                             const Point<float> &bottom,
                             const SingleRiverSettings& river,
                             utils::Array2D<uint8_t> &mask) const;

    /** Create bump sinusoidal palette according to channel width.
    @param [in] width - Channel width.
    @return Bump pallete according to distance to channel's center.
    */
    virtual std::vector<float> CreatePalette(int width);

    /** Detect point between 2 another ones (with some distortion).
    @param [in] top        - Current top point.
    @param [in] bottom     - Current bottom point.
    @param [in] distortion - Chanel maximal distortion.
    @return Middle point.
    */
    virtual Point<float> ChooseMiddlePoint(const Point<float>& top,
                                           const Point<float>& bottom,
                                           float distortion) const;

    /** Fill river mask with distances to channel's center.
    @param [in] river_mask  - Mask of distances to channel's center.
    @param [in] river_width - Channel width.
    */
    virtual void CreateChannelLadder(utils::Array2D<uint8_t>& river_mask,
                                     int river_width) const;

    /** Bump channel on height map.
    @param [in] mask    - Mask of distances to channel's center.
    @param [in] river   - River's settings.
    @param [in] bottom  - River's bottom value.
    @param [in] pallete - Bump pallete according to distance to channel's
    center.
    */
    virtual void BumpChannel(utils::Array2D<uint8_t>& mask,
                             const SingleRiverSettings& river,
                             float bottom,
                             const std::vector<float>& pallete) const;

    /** Fix bumped channel for deny river to go up.
    @param [in] x              - River's start point X coordinate.
    @param [in] y              - River's start point Y coordinate.
    @param [in] mask           - Mask of distances to channel's center.
    @param [in] prev_height    - Previous channels height.
    @param [in] channel_radius - Channel width.
    @param [in] bottom         - River's bottom value.
    @param [in] pallete        - Bump pallete according to distance to
                                 channel's center. */
    virtual void PostBumpChannel(int x,
                                 int y,
                                 utils::Array2D<uint8_t>& mask,
                                 float prev_height,
                                 int channel_radius,
                                 float bottom,
                                 const std::vector<float>& pallete) const;

    /** Random number generator. */
    utils::Random*                rand_ = nullptr;
    /** Minimal values and their positions for every chunk. */
    utils::Array2D<Point<float> > min_;
    /** Maximal values and their positions for every chunk. */
    utils::Array2D<Point<float> > max_;
    /** Possible river's last points. */
    std::vector<Point<float> >    last_points_;

 public:
    /** Height map mask from data storage. */
    utils::Array2D<float>* height_map_ = nullptr;
    /** River mask from data storage. */
    utils::Array2D<float>* river_mask_ = nullptr;
    /** Sea level mask from data storage. */
    float*                 sea_level_ = nullptr;

 protected:
    /** Settings for module. */
    struct {
        /** General settings. */
        GeneralSettings general;
        /** River settings. */
        RiverSettings   river;
        /** System settings. */
        SystemSettings  system;
    } settings_;
};

} // namescpace modules
} // namescpace prowogene

#endif // PROWOGENE_CORE_MODULES_RIVER_H_