#ifndef PROWOGENE_CORE_UTILS_IMAGE_IO_H_
#define PROWOGENE_CORE_UTILS_IMAGE_IO_H_

#include "utils/image.h"

namespace prowogene {
namespace utils {

/** @brief Image saving params. */
struct ImageIOParams {
    /** Image file format. */
    std::string format = "bmp";
    /** Image filename without extension. */
    std::string filename = "texture";
    /** Bit depth of output image. */
    int bit_depth = 24;
    /** Quality of output image (reserved). */
    int quality = 0;
};


/** @brief Image input/output worker. */
class ImageIO {
 public:
    /** Load image from file.
    @param [in] filename - Filename of image.
    @return decoded image. */
    virtual Image Load(const std::string& filename) const;

    /** Save image to file.
    @param [in] image  - Image to save.
    @param [in] params - Saving params. */
    virtual void Save(const Image& image, const ImageIOParams& params) const;

    /** Save height map to file.
    @param [in] hm     - Height map to save.
    @param [in] params - Saving params. */
    virtual void SaveHeightMap(const Array2D<float>& hm,
                               const ImageIOParams& params) const;

 protected:
    /** Load BMP image from file.
    @param [in] filename - Filename of image.
    @return decoded image. */
    virtual Image LoadBMP(const std::string& filename) const;

    /** Save BMP image to file.
    @param [in] image - Image to save.
    @param [in] par   - Saving params. */
    virtual void SaveBMP(const Image& image, const ImageIOParams& par) const;
};

} // namespace utils
} // namespace prowogene

#endif // PROWOGENE_CORE_UTILS_IMAGE_IO_H_
