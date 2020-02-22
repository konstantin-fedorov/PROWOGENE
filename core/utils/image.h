#ifndef PROWOGENE_CORE_UTILS_IMAGE_H_
#define PROWOGENE_CORE_UTILS_IMAGE_H_

#include <stdint.h>

#include <string>

#include "utils/array2d.h"

namespace prowogene {
namespace utils {

/** @brief Pixel in RGBA color space. */
struct RgbaPixel {
    /** Constructor. */
    RgbaPixel();

    /** Constructor.
    @param [in] r - Red component.
    @param [in] g - Green component.
    @param [in] b - Blue component.
    @param [in] a - Alpha component. */
    RgbaPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    /** Constructor.
    @param [in] hex_color - string with hex color. Can be filled with values
    like @c "#RRGGBBAA" , @c "#RRGGBB" , @c "RRGGBBAA" or @c "RRGGBB" . */
    RgbaPixel(std::string hex_color);

    /** Converts color to @c "#RRGGBBAA" format. */
    virtual std::string ToString() const;

    /** Red component. */
    uint8_t red   = 0;
    /** Green component. */
    uint8_t green = 0;
    /** Blue component. */
    uint8_t blue  = 0;
    /** Alpha component. */
    uint8_t alpha = 255;
};

using Image = Array2D<RgbaPixel>;

} // namespace utils
} // namespace prowogene

#endif // PROWOGENE_CORE_UTILS_IMAGE_H_
