#include "image.h"

namespace prowogene {
namespace utils {

using std::string;

RgbaPixel::RgbaPixel() { }

RgbaPixel::RgbaPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a) 
    : red(r)
    , green(g)
    , blue(b)
    , alpha(a) {
}

RgbaPixel::RgbaPixel(string hex_color) {
    if (!hex_color.size()) {
        return;
    }
    if (hex_color[0] == '#') {
        hex_color.erase(0, 1);
    }

    long long number = 0;
    try {
        number = std::stoll(hex_color, nullptr, 16);
    } catch(...) {
        return;
    }

    if (hex_color.size() > 6) {
        alpha = static_cast<uint8_t>(number & 0xFF);
        number >>= 8;
    }
    blue =  static_cast<uint8_t>(number & 0xFF);
    number >>= 8;
    green = static_cast<uint8_t>(number & 0xFF);
    number >>= 8;
    red =   static_cast<uint8_t>(number & 0xFF);
}

string RgbaPixel::ToString() const {
    const string hex_digits = "0123456789ABCDEF";
    string out = "#";
    out.reserve(9);
    out += hex_digits[red / 16];
    out += hex_digits[red % 16];
    out += hex_digits[green / 16];
    out += hex_digits[green % 16];
    out += hex_digits[blue / 16];
    out += hex_digits[blue % 16];
    out += hex_digits[alpha / 16];
    out += hex_digits[alpha % 16];
    return out;
}

} // namespace utils
} // namespace prowogene
