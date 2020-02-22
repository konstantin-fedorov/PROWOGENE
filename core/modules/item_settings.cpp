#include "item.h"

namespace prowogene {
namespace modules {

using std::string;
using utils::JsonValue;
using utils::JsonObject;
using utils::JsonArray;
using utils::JsonType;
using utils::InputString;
using IIS = ImportItemSettings;

static const string kImpItemFile =      "file";
static const string kImpItemId =        "id";
static const string kImpItemCount =     "count";
static const string kImpItemCountMin =  "min";
static const string kImpItemCountMax =  "max";
static const string kImpItemAngle =     "angle";
static const string kImpItemAngleMin =  "min";
static const string kImpItemAngleMax =  "max";
static const string kImpItemRadius =    "radius";
static const string kImpItemFrequency = "frequency";
static const string kImpItemLeveled =   "leveled";

void ImportItemSettings::Deserialize(JsonObject config) {
    file = config[kImpItemFile].Str();
    id =   config[kImpItemId];
    JsonObject json_count = config[kImpItemCount];
    count.min = json_count[kImpItemCountMin];
    count.max = json_count[kImpItemCountMax];
    JsonObject json_angle = config[kImpItemAngle];
    angle.min = json_angle[kImpItemAngleMin];
    angle.max = json_angle[kImpItemAngleMax];
    radius = config[kImpItemRadius];
    frequency = config[kImpItemFrequency];
    leveled = config[kImpItemLeveled];
}

JsonObject ImportItemSettings::Serialize() const {
    JsonObject config;
    config[kImpItemFile] = file;
    config[kImpItemId] = id;
    JsonObject json_count;
    json_count[kImpItemCountMin] = count.min;
    json_count[kImpItemCountMax] = count.max;
    config[kImpItemCount] = json_count;
    JsonObject json_angle;
    json_angle[kImpItemAngleMin] = angle.min;
    json_angle[kImpItemAngleMax] = angle.max;
    config[kImpItemAngle] = json_angle;
    config[kImpItemRadius] = radius;
    config[kImpItemFrequency] = frequency;
    config[kImpItemLeveled] = leveled;
    return config;
}

string ImportItemSettings::GetName() const {
    return kConfigImportItem;
}

ImportItemList ImportItemSettings::ReadFromFile(const string& filename) {
    JsonValue val;
    val.Parse(filename, InputString::FILENAME);
    if (val.GetType() != JsonType::ARRAY) {
        return ImportItemList();
    }
    const JsonArray arr = val;
    ImportItemList list;
    list.reserve(arr.size());
    for (const auto& elem : arr) {
        ImportItemSettings item;
        item.Deserialize(elem);
        list.push_back(item);
    }
    return list;
}

void ImportItemSettings::SaveToFile(const string& filename,
    const ImportItemList& settings) {
    JsonArray arr;
    arr.reserve(settings.size());
    for (const auto& item : settings) {
        arr.push_back(item.Serialize());
    }
    const JsonValue json = arr;
    json.Save(filename, true);
}


static const string kExpItemFile =  "file";
static const string kExpItemId =    "id";
static const string kExpItemX =     "x";
static const string kExpItemY =     "y";
static const string kExpItemZ =     "z";
static const string kExpItemAngle = "angle";

void ExportItemSettings::Deserialize(JsonObject config) {
    file = config[kExpItemFile].Str();
    id = config[kExpItemId];
    x = config[kExpItemX];
    y = config[kExpItemY];
    z = config[kExpItemZ];
    angle = config[kExpItemAngle];
}

JsonObject ExportItemSettings::Serialize() const {
    JsonObject config;
    config[kExpItemFile] = file;
    config[kExpItemId] = id;
    config[kExpItemX] = x;
    config[kExpItemY] = y;
    config[kExpItemZ] = z;
    config[kExpItemAngle] = angle;
    return config;
}

string ExportItemSettings::GetName() const {
    return kConfigExportItem;
}


static const string kExpChunkInfo =        "info";
static const string kExpChunkInfoModel =   "model";
static const string kExpChunkInfoTexture = "texture";
static const string kExpChunkInfoNormal =  "normal";
static const string kExpChunkInfoX =       "x";
static const string kExpChunkInfoY =       "y";
static const string kExpChunkItems =       "items";

void ExportChunkSettings::Deserialize(JsonObject config) {
    JsonObject info_config = config[kExpChunkInfo];
    info.files.model =   info_config[kExpChunkInfoModel].Str();
    info.files.texture = info_config[kExpChunkInfoTexture].Str();
    info.files.normal =  info_config[kExpChunkInfoNormal].Str();
    info.x =             info_config[kExpChunkInfoX];
    info.y =             info_config[kExpChunkInfoY];

    JsonArray json_items = config[kExpChunkItems];
    for (const auto& item : json_items) {
        ExportItemSettings placed;
        placed.Deserialize(item);
        items.push_back(placed);
    }
};

JsonObject ExportChunkSettings::Serialize() const {
    JsonObject config;
    JsonObject json_info;
    json_info[kExpChunkInfoModel] =   info.files.model;
    json_info[kExpChunkInfoTexture] = info.files.texture;
    json_info[kExpChunkInfoNormal] =  info.files.normal;
    json_info[kExpChunkInfoX] =       info.x;
    json_info[kExpChunkInfoY] =       info.y;
    config[kExpChunkInfo] = json_info;

    JsonArray json_items;
    json_items.reserve(items.size());
    for (const auto& item : items) {
        json_items.push_back(item.Serialize());
    }
    config[kExpChunkItems] = json_items;
    return config;
};

string ExportChunkSettings::GetName() const {
    return kConfigExportChunk;
};


static const string kExpWorldComplex =             "complex";
static const string kExpWorldComplexFiles =        "files";
static const string kExpWorldComplexFilesModel =   "model";
static const string kExpWorldComplexFilesTexture = "texture";
static const string kExpWorldComplexFilesNormal =  "normal";
static const string kExpWorldComplexSize =         "size";
static const string kExpWorldComplexItems =        "items";
static const string kExpWorldChunks =              "chunks";
static const string kExpWorldChunksSize =          "size";
static const string kExpWorldChunksCountX =        "count_x";
static const string kExpWorldChunksCountY =        "count_y";
static const string kExpWorldChunksData =          "data";
static const string kExpWorldWaterLevel =          "water_level";

void ExportWorldSettings::Deserialize(JsonObject config) {
    water_level = config[kExpWorldWaterLevel];
    JsonObject complex_config = config[kExpWorldComplex];
    JsonObject files_config = complex_config[kExpWorldComplexFiles];
    complex.files.model =   files_config[kExpWorldComplexFilesModel].Str();
    complex.files.texture = files_config[kExpWorldComplexFilesTexture].Str();
    complex.files.normal =  files_config[kExpWorldComplexFilesNormal].Str();
    complex.size = complex_config[kExpWorldComplexSize];
    JsonArray json_complex_items = config[kExpWorldComplexItems];
    for (const auto& item : json_complex_items) {
        ExportItemSettings placed;
        placed.Deserialize(item);
        complex.items.push_back(placed);
    }

    JsonObject chunks_config = config[kExpWorldChunks];
    chunks.size =    chunks_config[kExpWorldChunksSize];
    chunks.count_x = chunks_config[kExpWorldChunksCountX];
    chunks.count_y = chunks_config[kExpWorldChunksCountY];

    JsonArray json_data = chunks_config[kExpWorldChunksData];
    for (const auto& item : json_data) {
        ExportChunkSettings chunk_info;
        chunk_info.Deserialize(item);
        chunks.data.push_back(chunk_info);
    }
};

JsonObject ExportWorldSettings::Serialize() const {
    JsonObject config;
    config[kExpWorldWaterLevel] = water_level;
    JsonObject complex_config;
    complex_config[kExpWorldComplexFilesModel] =   complex.files.model;
    complex_config[kExpWorldComplexFilesTexture] = complex.files.texture;
    complex_config[kExpWorldComplexFilesNormal] =  complex.files.normal;
    complex_config[kExpWorldComplexSize] =         complex.size;
    JsonArray json_complex_items = complex_config[kExpWorldComplexItems];
    json_complex_items.reserve(complex.items.size());
    for (const auto& item : complex.items) {
        json_complex_items.push_back(item.Serialize());
    }
    complex_config[kExpWorldComplexItems] = json_complex_items;
    config[kExpWorldComplex] = complex_config;

    JsonObject chunks_config;
    chunks_config[kExpWorldChunksSize] =   chunks.size;
    chunks_config[kExpWorldChunksCountX] = chunks.count_x;
    chunks_config[kExpWorldChunksCountY] = chunks.count_y;
    JsonArray json_data;
    json_data.reserve(chunks.data.size());
    for (const auto& data : chunks.data) {
        json_data.push_back(data.Serialize());
    }
    chunks_config[kExpWorldChunksData] = json_data;
    config[kExpWorldChunks] = chunks_config;
    return config;
};

string ExportWorldSettings::GetName() const {
    return kConfigExportWorld;
};


static const string kItemEnabled =      "enabled";
static const string kItemConfig =       "config";
static const string kItemConfigFile =   "file";
static const string kItemConfigPretty = "pretty";
static const string kItemFullness =     "fullness";
static const string kItemForest =       "forest";
static const string kItemGlade =        "glade";
static const string kItemMountain =     "mountain";
static const string kItemRiver =        "river";
static const string kItemBeach =        "beach";
static const string kItemSea =          "sea";

void ItemSettings::Deserialize(JsonObject in_config) {
    enabled =       in_config[kItemEnabled];
    JsonObject json_config = in_config[kItemConfig];
    config.file =   json_config[kItemConfigFile].Str();
    config.pretty = json_config[kItemConfigPretty];
    fullness =      in_config[kItemFullness];
    file.forest =   in_config[kItemForest].Str();
    file.glade =    in_config[kItemGlade].Str();
    file.mountain = in_config[kItemMountain].Str();
    file.river =    in_config[kItemRiver].Str();
    file.beach =    in_config[kItemBeach].Str();
    file.sea =      in_config[kItemSea].Str();

    list.forest =   IIS::ReadFromFile(file.forest);
    list.glade =    IIS::ReadFromFile(file.glade);
    list.mountain = IIS::ReadFromFile(file.mountain);
    list.river =    IIS::ReadFromFile(file.river);
    list.beach =    IIS::ReadFromFile(file.beach);
    list.sea =      IIS::ReadFromFile(file.sea);
}

JsonObject ItemSettings::Serialize() const {
    IIS::SaveToFile(file.forest,   list.forest);
    IIS::SaveToFile(file.glade,    list.glade);
    IIS::SaveToFile(file.mountain, list.mountain);
    IIS::SaveToFile(file.river,    list.river);
    IIS::SaveToFile(file.beach,    list.beach);
    IIS::SaveToFile(file.sea,      list.sea);

    JsonObject out_config;
    out_config[kItemEnabled] =  enabled;
    JsonObject json_config;
    json_config[kItemConfigFile] =   config.file;
    json_config[kItemConfigPretty] = config.pretty;
    out_config[kItemConfig] = json_config;
    out_config[kItemFullness] = fullness;
    out_config[kItemForest] =   file.forest;
    out_config[kItemGlade] =    file.glade;
    out_config[kItemMountain] = file.mountain;
    out_config[kItemRiver] =    file.river;
    out_config[kItemBeach] =    file.beach;
    out_config[kItemSea] =      file.sea;
    return out_config;
}

string ItemSettings::GetName() const {
    return kConfigItem;
}

} // namescpace modules
} // namescpace prowogene
