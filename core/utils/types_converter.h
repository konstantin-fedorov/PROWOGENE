#ifndef PROWOGENE_CORE_UTILS_TYPES_CONVERTER_H_
#define PROWOGENE_CORE_UTILS_TYPES_CONVERTER_H_

#include <string>

#include "types.h"

namespace prowogene {
namespace utils {

/** @brief Converter class for types. */
class TypesConverter {
 public:
    /** Convert enumeration from string.
    @param [in] str - String value for convertion. */
    template <class T>
    static T To(const std::string& str);

    /** Convert string from enumeration.
    @param [in] val - Value for convertion. */
    template <class T>
    static std::string ToString(T val);

    /** Convert boime from int.
    @param [in] val - Value for convertion. */
    static Biome ToBiome(int val);

    /** Convert int from biome.
    @param [in] val - Value for convertion. */
    static int ToInt(Biome val);
};

} // namespace prowogene
} // namespace utils

#endif // PROWOGENE_CORE_UTILS_TYPES_CONVERTER_H_
