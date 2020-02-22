#include "image_io.h"

#include <string.h>

#include "utils/bmp.h"

namespace prowogene {
namespace utils {

using std::string;
using prowogene::utils::RgbaPixel;

Image ImageIO::Load(const std::string& filename) const {
    const string ext = filename.substr(filename.find_last_of(".") + 1);
    if (ext == "bmp") {
        return LoadBMP(filename);
    }
    return Image();
}

void ImageIO::Save(const Image& image, const ImageIOParams& params) const {
    if (params.format == "bmp") {
        SaveBMP(image, params);
    }
}

void ImageIO::SaveHeightMap(const Array2D<float>& hm,
        const ImageIOParams& params) const {
    const int width = hm.Width();
    const int height = hm.Height();
    Image image(width, height);
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            const uint8_t color = static_cast<uint8_t>(hm(x, y) * 255.0f);
            image(x, y) = RgbaPixel(color, color, color, 255);
        }
    }
    Save(image, params);
}

Image ImageIO::LoadBMP(const std::string& filename) const {
    Image decoded;
    Bmp::Decode(filename, decoded);
    return decoded;
}

void ImageIO::SaveBMP(const Image& image, const ImageIOParams& params) const {
    Bmp::Encode(params.filename + ".bmp", image, params.bit_depth);
}

} // namespace utils
} // namespace prowogene
