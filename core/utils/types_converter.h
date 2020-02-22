#ifndef PROWOGENE_CORE_UTILS_TYPES_CONVERTER_H_
#define PROWOGENE_CORE_UTILS_TYPES_CONVERTER_H_

#include <string>

#include "types.h"

namespace prowogene {
namespace utils {

/** @brief Converter class for types. */
class TypesConverter {
 public:
    /** Convert align from string.
    @param [in] str - String value for convertion. */
    static Align      ToAlign(const std::string& str);

    /** Convert distortion from string.
    @param [in] str - String value for convertion. */
    static Distortion ToDistortion(const std::string& str);

    /** Convert gradient from string.
    @param [in] str - String value for convertion. */
    static Gradient   ToGradient(const std::string& str);

    /** Convert key point from string.
    @param [in] str - String value for convertion. */
    static KeyPoint   ToKeyPoint(const std::string &str);

    /** Convert surface from string.
    @param [in] str - String value for convertion. */
    static Surface    ToSurface(const std::string& str);


    /** Convert string from align.
    @param [in] val - Value for convertion. */
    static std::string ToString(Align val);

    /** Convert string from distortion.
    @param [in] val - Value for convertion. */
    static std::string ToString(Distortion val);

    /** Convert string from gradient.
    @param [in] val - Value for convertion. */
    static std::string ToString(Gradient val);

    /** Convert string from key point.
    @param [in] val - Value for convertion. */
    static std::string ToString(KeyPoint val);

    /** Convert string from surface.
    @param [in] val - Value for convertion. */
    static std::string ToString(Surface val);


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
