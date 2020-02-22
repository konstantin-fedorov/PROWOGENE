#ifndef PROWOGENE_CORE_MODULES_ITEM_H_
#define PROWOGENE_CORE_MODULES_ITEM_H_

#include "general_settings.h"
#include "names_settings.h"
#include "system_settings.h"
#include "modules/model.h"
#include "modules/location.h"
#include "modules/texture.h"

namespace prowogene {
namespace modules {

/** Item settings config name. */
static const std::string kConfigItem =        "item";
/** External item import info config name. */
static const std::string kConfigImportItem =  "import_item";
/** External item export info config name. */
static const std::string kConfigExportItem =  "export_item";
/** Chunk export info config name. */
static const std::string kConfigExportChunk = "chunk";
/** Complex map export info config name. */
static const std::string kConfigExportWorld = "world";


/** @brief Set of filenames for 3D object. */
struct ExportFiles{
    /** 3D model name. */
    std::string model = "";
    /** Texture name for 3D model. */
    std::string texture = "";
    /** Normal map name for 3D model. */
    std::string normal = "";
};


struct ImportItemSettings;
using ImportItemList = std::vector<ImportItemSettings>;

/** @brief External item import preferences. */
struct ImportItemSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Read all items info from JSON file.
    @param [in] filename - Path to JSON with import info.
    @return List of item's import info. */
    static ImportItemList ReadFromFile(const std::string& filename);

    /** Save list of item's import info to JSON file.
    @param [in] filename - Path to JSON with import info.
    @param [in] settings - List with item's import info. */
    static void SaveToFile(const std::string& filename,
                           const ImportItemList& settings);

    /** 3D model filename. */
    std::string file = "";
    /** Id of entity - don't used inside generator, but can be helpful for
    objects with same 3D model but different related actions. */
    int id = 0;
    /** Count of items can be placed on the map. */
    struct {
        /** Minimal count. [0, ...). */
        int min = 0;
        /** Maximal count. [0, ...). -1 means unlimited count. */
        int max = -1;
    } count;
    /** Angle in degrees on that object can be rotated (clockwize). */
    struct {
        /** Minimal angle. [0, 360]. */
        int min = 0;
        /** Maximal angle. [0, 360]. */
        int max = 0;
    } angle;
    /** Frequency of item placing. Related to other items. [0.0, 1.0]. */
    float frequency = 0.0f;
    /** Radius (in 3D units) where no new instances wouldn't placed. */
    float radius = 0.0f;
    /** Need to create flat area near item or not. */
    bool  leveled = false;
};


/** @brief External item export preferences. */
struct ExportItemSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** @copydoc ImportItemSettings::file */
    std::string file = "";
    /** @copydoc ImportItemSettings::id */
    int   id = 0;
    /** X coordinate of item (in 3D units). */
    float x = 0.0f;
    /** Y coordinate of item (in 3D units). */
    float y = 0.0f;
    /** Z coordinate of item (in 3D units). */
    float z = 0.0f;
    /** Rotation angle in degrees (clockwize). */
    float angle = 0.0f;
};


/** @brief Chunk export preferences. */
struct ExportChunkSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Chunk 3D model information. */
    struct {
        /** Filenames for chunk 3D model. */
        ExportFiles files;
        /** X index of chunk. */
        int         x = 0;
        /** Y index of chunk. */
        int         y = 0;
    } info;
    /** Information about items placed on that chunk. */
    std::list<ExportItemSettings> items;
};


/** @brief Complex world export preferences. */
struct ExportWorldSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Info for export complex map as single 3D model. */
    struct {
        /** Filenames for complex world model. */
        ExportFiles files;
        /** Map size in 3D units. */
        float size = 0.0f;
        /** Information about all placed items. */
        std::list<ExportItemSettings> items;
    } complex;
    /** Info for export complex map as set of chunks. */
    struct {
        /** Chunk size in 3D units. */
        float size = 0.0f;
        /** Chunks count per X coord. */
        int   count_x = 0;
        /** Chunks count per Y coord. */
        int   count_y = 0;
        /** Chunks export data. */
        std::list<ExportChunkSettings> data;
    } chunks;
    /** Water level of map. */
    float water_level = 0.0f;
};


/** @brief Item module settings. */
struct ItemSettings : ISettings {
    /** @copydoc ISettings::Deserialize */
    void Deserialize(utils::JsonObject config) override;
    /** @copydoc ISettings::Serialize */
    utils::JsonObject Serialize() const override;
    /** @copydoc ISettings::GetName */
    std::string GetName() const override;

    /** Item placing enabled. Any. */
    bool enabled = false;
    /** Periodicity of item placing. [0.0, 1.0]. */
    float fullness = 0.0f;
    /** Output JSON file with export info. */
    struct {
        /** JSON filename. */
        std::string file = "world.json";
        /** JSON saving mode. Pretty is more readable but big. */
        bool        pretty = false;
    } config;
    /** Items to place in locations. */
    struct {
        /** Forest items. */
        ImportItemList forest = { };
        /** Glade items. */
        ImportItemList glade = { };
        /** Mountain items. */
        ImportItemList mountain = { };
        /** River items. */
        ImportItemList river = { };
        /** Beach items. */
        ImportItemList beach = { };
        /** Sea items. */
        ImportItemList sea = { };
    } list;
    /** Files with info about items to place in locations. */
    struct {
        /** Forest items. */
        std::string forest = "";
        /** Glade items. */
        std::string glade = "";
        /** Mountain items. */
        std::string mountain = "";
        /** River items. */
        std::string river = "";
        /** Beach items. */
        std::string beach = "";
        /** Sea items. */
        std::string sea = "";
    } file;
};


/** @brief Module for placing external items on map.

Places external items on the map according to their settings. */
class ItemModule : public IModule {
 public:
    /** @copydoc IModule::SetStorage */
    void SetStorage(Storage* storage) override;
    /** @copydoc IModule::Init */
    void Init() override;
    /** @copydoc IModule::Process */
    bool Process() override;
    /** @copydoc IModule::GetNeededData */
    std::list<std::string> GetNeededData() const override;
    /** @copydoc IModule::GetNeededSettings */
    std::list<std::string> GetNeededSettings() const override;
    /** @copydoc IModule::ApplySettings */
    void ApplySettings(ISettings *settings) override;
    /** @copydoc IModule::Deinit */
    void Deinit() override;
    /** @copydoc IModule::GetName */
    std::string GetName() const override;

 protected:
    /** Place items from list to map values that in selected location.
    @param [in] info     - List with items for selected location.
    @param [in] location - Selected location.
    @return @c true when placing is succeded, @c false otherwise. */
    virtual bool PlaceLocation(const ImportItemList& info, Location location);

    /** Update object_mask_ borders to prevent placing objects at borders. */
    virtual void ClearBorders();

    /** Fill object_mask_ with distances to nearest unsuitable map element*/
    virtual void CreateWave();

    /** Update filled with wave object_mask_ after iten was added to it.
    @param [in] x_center    - X coordinate of placed item's center.
    @param [in] y_center    - Y coordinate of placed item's center.
    @param [in] item_radius - Item radius in points.
    @param [in] old_val     - Previous value in selected point. */
    virtual void UpdateWave(int x_center,
                            int y_center,
                            int item_radius,
                            int old_val);

    /** Separate single item list to necessary and unnecessary placed items.
    @param [in] all       - List with all import items.
    @param [out] required - List with items that must be placed necessarily.
    @param [out] optional - List with items that can be placed optionaly. */
    virtual void SeparateRequiredAndOptional(const ImportItemList& all,
                                             ImportItemList& required,
                                             ImportItemList& optional);

    /** Place all items that must be placed necessary.
    @param [in] required - List with items that must be placed necessarily.
    @return @c true if all items was correctly placed, @c false otherwise. */
    virtual bool PlaceRequired(const ImportItemList& required);

    /** Place all items that can be placed optionaly.
    @param [in] optional - list with items that can be placed optionaly.
    */
    virtual void PlaceOptional(const ImportItemList& optional);

    /** Place item on map and optionaly mark object_mask_ where it was placed.
    @param [in] item       - Import item information.
    @param [in] center     - Coordinate of placed item's center.
    @param [in] clean_mask - Mark object_mask_ where item was placed with
                             values for deprecate placing other items. */
    virtual void PlaceItem(const ImportItemSettings& item,
                           const Point<int>& center,
                           bool clean_mask);

    /** Fill export information about all world according to placed items.
    @return Export information about all world. */
    virtual ExportWorldSettings CreateExportInfo();

    /** Fill export file filenames with valid values.
    @param [out] files          - Filenames to fill.
    @param [in] model           - 3D model name without extension.
    @param [in] texture_enabled - Fill texture value or not.
    @param [in] texture         - Texture map name without extension.
    @param [in] normal          - Normal map name without extension. */
    virtual void FillExportFiles(ExportFiles& files,
                                 const std::string &model,
                                 bool texture_enabled,
                                 const std::string &texture,
                                 const std::string &normal) const;

    /** Random number generator. */
    utils::Random                 rand_ = utils::Random(0);
    /** Object mask with distances to nearest value deprecated for placing. */
    utils::Array2D<int>           object_mask_;
    /** List of placed ofjects for all world. */
    std::list<ExportItemSettings> placed_objects_;

    /** Height map from data storage. */
    utils::Array2D<float>*    height_map_ = nullptr;
    /** Location map from data storage. */
    utils::Array2D<Location>* location_map_ = nullptr;
    /** Sea level from data storage. */
    float*                    sea_level_ = nullptr;

    /** Settings for module. */
    struct {
        /** Item settings. */
        ItemSettings     item;
        /** General settings. */
        GeneralSettings  general;
        /** Location settings. */
        LocationSettings location;
        /** Model settings. */
        ModelSettings    model;
        /** Names settings. */
        NamesSettings    names;
        /** System settings. */
        SystemSettings   system;
        /** Texture settings. */
        TextureSettings  texture;
    } settings_;
};

} // namescpace modules
} // namescpace prowogene

#endif // PROWOGENE_CORE_MODULES_ITEM_H_
