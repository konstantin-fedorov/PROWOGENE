#ifndef PROWOGENE_CORE_UTILS_BMP_H_
#define PROWOGENE_CORE_UTILS_BMP_H_

#include "utils/image.h"

namespace prowogene {
namespace utils {

/** Unsigned 8bit integer. */
using BYTE =  uint8_t;
/** Unsigned 16bit integer. */
using WORD =  uint16_t;
/** Unsigned 32bit integer. */
using DWORD = uint32_t;
/** Signed 32bit integer. */
using LONG =  int32_t;
/** File dumped data. */
using FileDump = std::vector<uint8_t>;


/** @brief Information about the type, size, and layout of a DIB file. */
typedef struct BITMAPFILEHEADER {
    WORD  bfType;
    DWORD bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER;


/** @brief Information about the dimensions and color format for DIB file. */
typedef struct BITMAPCOREHEADER {
    DWORD bcSize;
    WORD  bcWidth;
    WORD  bcHeight;
    WORD  bcPlanes;
    WORD  bcBitCount;
} BITMAPCOREHEADER;


/** @brief Information about the dimensions and color format of a DIB. */
typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG  biWidth;
    LONG  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG  biXPelsPerMeter;
    LONG  biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER;


/** @brief Extended BITMAPINFOHEADER structure. */
typedef struct tagBITMAPV5HEADER {
    DWORD bV5Size;
    LONG  bV5Width;
    LONG  bV5Height;
    WORD  bV5Planes;
    WORD  bV5BitCount;
    DWORD bV5Compression;
    DWORD bV5SizeImage;
    LONG  bV5XPelsPerMeter;
    LONG  bV5YPelsPerMeter;
    DWORD bV5ClrUsed;
    DWORD bV5ClrImportant;
    DWORD bV5RedMask;
    DWORD bV5GreenMask;
    DWORD bV5BlueMask;
    DWORD bV5AlphaMask;
    DWORD bV5CSType;
    // There must be some rarely used info.
} BITMAPV5HEADER;


/** @brief BMP files encoding and decoding support.

Supports decoding of 8-, 24- and 32 bit BMP.
Supports encoding of 24- and 32- bit BMP. */
class Bmp {
 public:
    /** Save image to BMP file.
    @param [in] file - Filename to save image.
    @param [in] data - Data for saving.
    @param [in] bits - Bit depth of output image. */
    static void Encode(const std::string& file, const Image &data, int bits);

    /** Read image from BMP file.
    @param [in] file  - Filename for reading image.
    @param [out] data - Output image.
    @return @c true if decoding is succeeded, @c false otherwise. */
    static bool Decode(const std::string& file, Image& data);

 protected:
    /** Flag for usage RGB 8bit. */
    static const int kBiRGB = 0;
    /** Flag for custom RGB mask. */
    static const int kBiBitFields = 3;
    /** Flag for custom RGBA mask. */
    static const int kBiAlphaBitFields = 6;

    /** Size of serialized BITMAPFILEHEADER. */
    static const int kBitmapFileHeaderSize = 14;
    /** Size of serialized BITMAPCOREHEADER. */
    static const int kBitmapCoreHeaderSize = 12;
    /** Size of serialized BITMAPINFOHEADER. */
    static const int kBitmapInfoHeaderSize = 40;
    /** Size of serialized BITMAPV5HEADER. */
    static const int kBitmapV5HeaderSize =   124;
    /** Size of rarely used BITMAPV5HEADER elements in the end of header. */
    static const int kBitmapV5HeaderRareFeaturesSize = 20;


    /** Read 8 bit BMP file.
    @param [out] data     - Output image.
    @param [in] file_dump - Dumped to memory file.
    @param [in] bfh       - File header.
    @param [in] bih       - Info header. */
    static void Read8BitColors(Image& data,
                               const FileDump& file_dump,
                               const BITMAPFILEHEADER& bfh,
                               const BITMAPINFOHEADER& bih);

    /** Read pixel palette for 8 bit image.
    @param [in] file_dump - Dumped to memory file.
    @param [in] bih       - Info header.
    @return Pixel palette. */
    static std::vector<RgbaPixel> ReadPalette(const FileDump& file_dump,
                                              const BITMAPINFOHEADER& bih);

    /** Read 24 bit BMP file.
    @param [out] data     - Output image.
    @param [in] file_dump - Dumped to memory file.
    @param [in] bfh       - File header.
    @param [in] bih       - Info header. */
    static void Read24BitColors(Image& data,
                                const FileDump& file_dump,
                                const BITMAPFILEHEADER& bfh,
                                const BITMAPINFOHEADER& bih);

    /** Read 32 bit BMP file.
    @param [out] data     - Output image.
    @param [in] file_dump - Dumped to memory file.
    @param [in] bfh       - File header.
    @param [in] bih       - Info header. */
    static void Read32BitColors(Image& data,
                                const FileDump& file_dump,
                                const BITMAPFILEHEADER& bfh,
                                const BITMAPINFOHEADER& bih);

    /** Save image to 24 bit BMP file.
    @param [in] file - Filename to save image.
    @param [in] data - Data for saving. */
    static void Write24BitBmp(const std::string& file, const Image& data);

    /** Save image to 32 bit BMP file.
    @param [in] file - Filename to save image.
    @param [in] data - Data for saving. */
    static void Write32BitBmp(const std::string& file, const Image& data);
};

} // namespace utils
} // namespace prowogene

#endif // PROWOGENE_CORE_UTILS_BMP_H_
