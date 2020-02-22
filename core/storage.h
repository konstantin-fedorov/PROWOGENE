#ifndef PROWOGENE_CORE_STORAGE_H_
#define PROWOGENE_CORE_STORAGE_H_

#include <map>
#include <string>

#include "utils/array2d.h"
#include "utils/random.h"

namespace prowogene {

/** That keys are using inside core modules. For new modules, don't set same
names as below. */

/** Beach level data text key. */
static const std::string kStorageBeachLevel =   "beach_level";
/** Height map data text key. */
static const std::string kStorageHeightMap =    "height_map";
/** Location map data text key. */
static const std::string kStorageLocationMap =  "location_map";
/** Mountain mask data text key. */
static const std::string kStorageMountainMask = "mountain_mask";
/** River mask data text key. */
static const std::string kStorageRiverMask =    "river_mask";
/** Sea level data text key. */
static const std::string kStorageSeaLevel =     "sea_level";
/** Model input/output instance text key. */
static const std::string kStorageModelIO =      "model_io";
/** Image input/output instance text key. */
static const std::string kStorageImageIO =      "image_io";


/**@brief Data storage helper class.

Provides access to data by key. Can store any types of data but don't stores
any information about it's type. */
class Storage {
 public:
    /** Add data to storage.
    @param [in] key  - Text key for data access.
    @param [in] data - Stored data itself. */
    void AddData(const std::string& key, void* data) {
        data_[key] = data;
    }

    /** Delete data in storage.
    @param [in] key - Text key for data that will be deleted. */
    template <typename T>
    void RemoveData(const std::string& key) {
        void* data = GetData<T>(key);
        if (data) {
            T* real_data = (T*)data;
            delete real_data;
            data = nullptr;
        }
    }

    /** Get data pointer from storage.
    @param [in] key - Text key for data that will be deleted.
    @return Pointer to data or @c nullptr if there is no such data with that
            key. */
    template <typename T>
    T* GetData(const std::string& key) {
        auto it = data_.find(key);
        if (it == data_.end()) {
            return nullptr;
        } else {
            return (T*)it->second;
        }
    }

 protected:
    /** Data storage itself. There can be any types, so they all are storing
    as @c void* */
    std::map<std::string, void*> data_;
};

} // namespace prowogene

#endif // PROWOGENE_CORE_STORAGE_H_
