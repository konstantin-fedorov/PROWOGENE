#include "bmp.h"

#include <string.h>
#include <fstream>

namespace prowogene {
namespace utils {

using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
using prowogene::utils::Array2D;
using prowogene::utils::Image;
using prowogene::utils::RgbaPixel;


template <typename Type>
static void Read(const vector<uint8_t>& data, int &pos, Type &result) {
    memcpy(reinterpret_cast<char*>(&result), data.data() + pos, sizeof(Type));
    pos += sizeof(Type);
}

template <typename Type>
static void Write(ofstream& file, Type value) {
    file.write((char*)&value, sizeof(value));
}

static inline int GetPadding(int width, int bit_depth) {
    width *= bit_depth / 8;
    if (width % 4 != 0) {
        return 4 - width % 4;
    } else {
        return 0;
    }
}


void Bmp::Encode(const string& file, const Image &data, int bits) {
    switch (bits) {
    case 32:
        WriteBmp<32>(file, data);
        break;
    case 24:
    default:
        WriteBmp<24>(file, data);
        break;
    }
};

bool Bmp::Decode(const string& filename, Image &data) {
    ifstream file(filename, ifstream::binary);
    if (!file.is_open()) {
        return false;
    }

    file.seekg(0, std::ios::end);
    const size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    FileDump file_dump(size);
    file.read((char*)file_dump.data(), size);
    file.close();

    BITMAPFILEHEADER bfh;
    int pos = 0;
    Read(file_dump, pos, bfh.bfType);
    Read(file_dump, pos, bfh.bfSize);
    Read(file_dump, pos, bfh.bfReserved1);
    Read(file_dump, pos, bfh.bfReserved2);
    Read(file_dump, pos, bfh.bfOffBits);

    BITMAPINFOHEADER bih;
    DWORD red_mask = 0;
    DWORD green_mask = 0;
    DWORD blue_mask = 0;
    DWORD alpha_mask = 0;
    DWORD info_version = 0;
    Read(file_dump, pos, info_version);

    switch (info_version) {
    case kBitmapCoreHeaderSize : {
        BITMAPCOREHEADER bch;
        bch.bcSize = info_version;
        Read(file_dump, pos, bch.bcWidth);
        Read(file_dump, pos, bch.bcHeight);
        Read(file_dump, pos, bch.bcPlanes);
        Read(file_dump, pos, bch.bcBitCount);

        int align = 0;
        if (bch.bcWidth % 4 != 0) {
            align = 4 - bch.bcWidth % 4;
        }

        bih.biWidth =  std::abs(bch.bcWidth);
        bih.biHeight = std::abs(bch.bcHeight);
        bih.biPlanes = bch.bcPlanes;
        bih.biBitCount = bch.bcBitCount;
        bih.biCompression = kBiRGB;
        bih.biSizeImage = (bch.bcWidth + align) * std::abs(bch.bcHeight);
        bih.biXPelsPerMeter = 2000;
        bih.biYPelsPerMeter = 2000;
        bih.biClrUsed = 0;
        bih.biClrImportant = 0;
        break;
    }
    default:
        bih.biSize = info_version;
        Read(file_dump, pos, bih.biWidth);
        Read(file_dump, pos, bih.biHeight);
        Read(file_dump, pos, bih.biPlanes);
        Read(file_dump, pos, bih.biBitCount);
        Read(file_dump, pos, bih.biCompression);
        Read(file_dump, pos, bih.biSizeImage);
        Read(file_dump, pos, bih.biXPelsPerMeter);
        Read(file_dump, pos, bih.biYPelsPerMeter);
        Read(file_dump, pos, bih.biClrUsed);
        Read(file_dump, pos, bih.biClrImportant);
        if (bih.biCompression == kBiBitFields) {
            Read(file_dump, pos, red_mask);
            Read(file_dump, pos, green_mask);
            Read(file_dump, pos, blue_mask);
        } else if (bih.biCompression == kBiAlphaBitFields) {
            Read(file_dump, pos, red_mask);
            Read(file_dump, pos, green_mask);
            Read(file_dump, pos, blue_mask);
            Read(file_dump, pos, alpha_mask);
        }
    }

    switch (bih.biBitCount) {
    case 8:
        data = ReadColors<8>(file_dump, bih.biWidth, bih.biHeight, bfh.bfOffBits, bih.biSize);
        break;
    case 24:
        data = ReadColors<24>(file_dump, bih.biWidth, bih.biHeight, bfh.bfOffBits, bih.biSize);
        break;
    case 32:
        data = ReadColors<32>(file_dump, bih.biWidth, bih.biHeight, bfh.bfOffBits, bih.biSize);
        break;
    default:
        return false;
    }

    return true;
}

template <int BIT_COUNT>
static Image Bmp::ReadColors(const FileDump& file_dump,
                             const int width,
                             const int height,
                             int pixel_data_pos,
                             const int info_header_size) {
    const int padding = GetPadding(width, BIT_COUNT);
    Image data(width, height);

    const vector<RgbaPixel> palette = (BIT_COUNT == 8) ?
                                      ReadPalette(file_dump, info_header_size) :
                                      vector<RgbaPixel>();

    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            if (BIT_COUNT == 8) {
                uint8_t index = 0;
                Read(file_dump, pixel_data_pos, index);
                data(x, y).red   = palette[index].red;
                data(x, y).green = palette[index].green;
                data(x, y).blue  = palette[index].blue;
                data(x, y).alpha = palette[index].alpha;
            } else {
                Read(file_dump, pixel_data_pos, data(x, y).blue);
                Read(file_dump, pixel_data_pos, data(x, y).green);
                Read(file_dump, pixel_data_pos, data(x, y).red);
                if (BIT_COUNT == 32) {
                    Read(file_dump, pixel_data_pos, data(x, y).alpha);
                }
            }
        }
        for (int i = 0; i < padding; ++i) {
            uint8_t offset_bytes = 0;
            Read(file_dump, pixel_data_pos, offset_bytes);
        }
    }

    return data;
}

vector<RgbaPixel> Bmp::ReadPalette(const FileDump& file_dump,
                                   const int info_header_size) {
    int pos = kBitmapFileHeaderSize + info_header_size;
    vector<RgbaPixel> palette;
    static const int palette_size = 256;
    palette.resize(palette_size);
    for (size_t i = 0; i < palette_size; i++) {
        Read(file_dump, pos, palette[i].blue);
        Read(file_dump, pos, palette[i].green);
        Read(file_dump, pos, palette[i].red);
        if (info_header_size == kBitmapCoreHeaderSize) {
            palette[i].alpha = 255;
        } else {
            Read(file_dump, pos, palette[i].alpha);
        }
    }
    return palette;
}

template <int BIT_COUNT>
void Bmp::WriteBmp(const string& filename, const Image& data) {
    ofstream file;
    file.open(filename, std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    static constexpr int info_header_size = (BIT_COUNT == 24) ? kBitmapInfoHeaderSize : kBitmapV5HeaderSize;
    const int width = data.Width();
    const int height = data.Height();
    const int padding = (BIT_COUNT == 24) ? GetPadding(width, 24) : 0;
    const int image_size = (width * (BIT_COUNT / 8) + padding) * height;

    BITMAPFILEHEADER bfh;
    bfh.bfType = static_cast<WORD>(0x4D42);
    bfh.bfSize = kBitmapFileHeaderSize + info_header_size + image_size;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits = kBitmapFileHeaderSize + info_header_size;

    Write(file, bfh.bfType);
    Write(file, bfh.bfSize);
    Write(file, bfh.bfReserved1);
    Write(file, bfh.bfReserved2);
    Write(file, bfh.bfOffBits);

    if (BIT_COUNT == 24) {
        BITMAPINFOHEADER bih;
        bih.biSize          = info_header_size;
        bih.biWidth         = width;
        bih.biHeight        = height;
        bih.biPlanes        = 1;
        bih.biBitCount      = BIT_COUNT;
        bih.biCompression   = kBiRGB;
        bih.biSizeImage     = image_size;
        bih.biXPelsPerMeter = 2000;
        bih.biYPelsPerMeter = 2000;
        bih.biClrUsed       = 0;
        bih.biClrImportant  = 0;

        Write(file, bih.biSize);
        Write(file, bih.biWidth);
        Write(file, bih.biHeight);
        Write(file, bih.biPlanes);
        Write(file, bih.biBitCount);
        Write(file, bih.biCompression);
        Write(file, bih.biSizeImage);
        Write(file, bih.biXPelsPerMeter);
        Write(file, bih.biYPelsPerMeter);
        Write(file, bih.biClrUsed);
        Write(file, bih.biClrImportant);
    } else {
        BITMAPV5HEADER bv5h;
        bv5h.bV5Size          = info_header_size;
        bv5h.bV5Width         = width;
        bv5h.bV5Height        = height;
        bv5h.bV5Planes        = 1;
        bv5h.bV5BitCount      = BIT_COUNT;
        bv5h.bV5Compression   = kBiBitFields;
        bv5h.bV5SizeImage     = image_size;
        bv5h.bV5XPelsPerMeter = 2000;
        bv5h.bV5YPelsPerMeter = 2000;
        bv5h.bV5ClrUsed       = 0;
        bv5h.bV5ClrImportant  = 0;
        bv5h.bV5RedMask       = 0x00FF0000U;
        bv5h.bV5GreenMask     = 0x0000FF00U;
        bv5h.bV5BlueMask      = 0x000000FFU;
        bv5h.bV5AlphaMask     = 0xFF000000U;
        bv5h.bV5CSType        = ((uint32_t)('s') << 24) +
                                ((uint32_t)('R') << 16) +
                                ((uint32_t)('G') << 8) +
                                ((uint32_t)('B') << 0);

        Write(file, bv5h.bV5Size);
        Write(file, bv5h.bV5Width);
        Write(file, bv5h.bV5Height);
        Write(file, bv5h.bV5Planes);
        Write(file, bv5h.bV5BitCount);
        Write(file, bv5h.bV5Compression);
        Write(file, bv5h.bV5SizeImage);
        Write(file, bv5h.bV5XPelsPerMeter);
        Write(file, bv5h.bV5YPelsPerMeter);
        Write(file, bv5h.bV5ClrUsed);
        Write(file, bv5h.bV5ClrImportant);
        Write(file, bv5h.bV5RedMask);
        Write(file, bv5h.bV5GreenMask);
        Write(file, bv5h.bV5BlueMask);
        Write(file, bv5h.bV5AlphaMask);
        Write(file, bv5h.bV5CSType);

        for (int i = 0; i < kBitmapV5HeaderRareFeaturesSize; ++i) {
            Write(file, (uint8_t)0);
        }
    }

    vector<uint8_t> raw_image(image_size);
    size_t index = 0;
    for (int j = height - 1; j >= 0; --j) {
        for (int i = 0; i < width; ++i) {
            raw_image[index + 0] = data(i, j).blue;
            raw_image[index + 1] = data(i, j).green;
            raw_image[index + 2] = data(i, j).red;
            if (BIT_COUNT == 24) {
                index += 3;
            } else {
                raw_image[index + 3] = data(i, j).alpha;
                index += 4;
            }
        }
        if (BIT_COUNT == 24) {
            for (int k = 0; k < padding; ++k) {
                raw_image[index] = 0;
                ++index;
            }
        }
    }

    file.write((char*)raw_image.data(), raw_image.size());
    file.close();
}

} // namespace utils
} // namespace prowogene
