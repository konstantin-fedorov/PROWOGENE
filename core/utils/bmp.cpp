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
    case 24:
        Write24BitBmp(file, data);
        break;
    case 32:
        Write32BitBmp(file, data);
        break;
    default:
        Write24BitBmp(file, data);
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
        Read8BitColors(data, file_dump, bfh, bih);
        break;
    case 24:
        Read24BitColors(data, file_dump, bfh, bih);
        break;
    case 32:
        Read32BitColors(data, file_dump, bfh, bih);
        break;
    default:
        return false;
    }

    return true;
}

void Bmp::Read8BitColors(Image& data, const FileDump& file_dump,
        const BITMAPFILEHEADER& bfh, const BITMAPINFOHEADER& bih) {
    const int padding = GetPadding(bih.biWidth, 8);
    data.Resize(bih.biWidth, bih.biHeight);

    vector<RgbaPixel> palette = ReadPalette(file_dump, bih);

    int pos = bfh.bfOffBits;
    for (int y = bih.biHeight - 1; y >= 0; --y) {
        uint8_t index = 0;
        for (int x = 0; x < bih.biWidth; ++x) {
            Read(file_dump, pos, index);
            data(x, y).red =   palette[index].red;
            data(x, y).green = palette[index].green;
            data(x, y).blue =  palette[index].blue;
            data(x, y).alpha = palette[index].alpha;
        }
        for (int i = 0; i < padding; ++i) {
            Read(file_dump, pos, index);
        }
    }
}

vector<RgbaPixel> Bmp::ReadPalette(const FileDump& file_dump,
        const BITMAPINFOHEADER& bih) {
    int pos = kBitmapFileHeaderSize + bih.biSize;
    vector<RgbaPixel> palette;
    static const int palette_size = 256;
    palette.resize(palette_size);
    for (size_t i = 0; i < palette_size; i++) {
        Read(file_dump, pos, palette[i].blue);
        Read(file_dump, pos, palette[i].green);
        Read(file_dump, pos, palette[i].red);
        if (bih.biSize == kBitmapCoreHeaderSize) {
            palette[i].alpha = 255;
        } else {
            Read(file_dump, pos, palette[i].alpha);
        }
    }
    return palette;
}

void Bmp::Read24BitColors(Image& data, const FileDump& file_dump,
        const BITMAPFILEHEADER& bfh, const BITMAPINFOHEADER& bih) {
    const int padding = GetPadding(bih.biWidth, 24);
    data.Resize(bih.biWidth, bih.biHeight);

    int pos = bfh.bfOffBits;
    for (int y = bih.biHeight - 1; y >= 0; --y) {
        uint8_t offset_bytes = 0;
        for (int x = 0; x < bih.biWidth; ++x) {
            Read(file_dump, pos, data(x, y).blue);
            Read(file_dump, pos, data(x, y).green);
            Read(file_dump, pos, data(x, y).red);
        }
        for (int i = 0; i < padding; ++i) {
            Read(file_dump, pos, offset_bytes);
        }
    }
}

void Bmp::Read32BitColors(Image& data, const FileDump& file_dump,
        const BITMAPFILEHEADER& bfh, const BITMAPINFOHEADER& bih) {
    data.Resize(bih.biWidth, bih.biHeight);

    int pos = bfh.bfOffBits;
    for (int y = bih.biHeight - 1; y >= 0; --y) {
        for (int x = 0; x < bih.biWidth; ++x) {
            Read(file_dump, pos, data(x, y).blue);
            Read(file_dump, pos, data(x, y).green);
            Read(file_dump, pos, data(x, y).red);
            Read(file_dump, pos, data(x, y).alpha);
        }
    }
}

void Bmp::Write24BitBmp(const string& filename, const Image& data) {
    ofstream file;
    file.open(filename, std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;

    const int padding = GetPadding(data.Width(), 24);

    bih.biSize = kBitmapInfoHeaderSize;
    bih.biWidth = data.Width();
    bih.biHeight = data.Height();
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = kBiRGB;
    bih.biSizeImage = (bih.biWidth * 3 + padding) * bih.biHeight;
    bih.biXPelsPerMeter = 2000;
    bih.biYPelsPerMeter = 2000;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;
    
    bfh.bfType = static_cast<WORD>(0x4D42);
    bfh.bfSize = kBitmapFileHeaderSize +
                 kBitmapInfoHeaderSize +
                 (bih.biWidth * 3 + padding) * bih.biHeight;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits = kBitmapFileHeaderSize + kBitmapInfoHeaderSize;

    Write(file, bfh.bfType);
    Write(file, bfh.bfSize);
    Write(file, bfh.bfReserved1);
    Write(file, bfh.bfReserved2);
    Write(file, bfh.bfOffBits);

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

    vector<uint8_t> raw_image(bih.biSizeImage);
    int index = 0;
    for (int j = bih.biHeight - 1; j >= 0; --j) {
        for (int i = 0; i < bih.biWidth; ++i) {
            raw_image[index + 0] = data(i, j).blue;
            raw_image[index + 1] = data(i, j).green;
            raw_image[index + 2] = data(i, j).red;
            index += 3;
        }
        for (int k = 0; k < padding; ++k) {
            raw_image[index] = 0;
            ++index;
        }
    }
    file.write((char*)raw_image.data(), raw_image.size());
    file.close();
} 

void Bmp::Write32BitBmp(const string& filename, const Image& data) {
    ofstream file;
    file.open(filename, std::ios::binary);
    if (!file.is_open()) {
        return;
    }
    BITMAPFILEHEADER bfh;
    BITMAPV5HEADER bv5h;

    bv5h.bV5Size = kBitmapV5HeaderSize;
    bv5h.bV5Width = data.Width();
    bv5h.bV5Height = data.Height();
    bv5h.bV5Planes = 1;
    bv5h.bV5BitCount = 32;
    bv5h.bV5Compression = kBiBitFields;
    bv5h.bV5SizeImage = (bv5h.bV5Width * 4) * bv5h.bV5Height;
    bv5h.bV5XPelsPerMeter = 2000;
    bv5h.bV5YPelsPerMeter = 2000;
    bv5h.bV5ClrUsed = 0;
    bv5h.bV5ClrImportant = 0;

    bv5h.bV5RedMask   = 0x00FF0000U;
    bv5h.bV5GreenMask = 0x0000FF00U;
    bv5h.bV5BlueMask  = 0x000000FFU;
    bv5h.bV5AlphaMask = 0xFF000000U;
    bv5h.bV5CSType = ((uint32_t)('s') << 24) +
                     ((uint32_t)('R') << 16) +
                     ((uint32_t)('G') << 8) +
                     ((uint32_t)('B') << 0);

    bfh.bfType = (WORD)0x4D42;
    bfh.bfSize = kBitmapFileHeaderSize +
                 kBitmapV5HeaderSize +
                 (bv5h.bV5Width * 4) * bv5h.bV5Height;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits = kBitmapFileHeaderSize + kBitmapV5HeaderSize;

    Write(file, bfh.bfType);
    Write(file, bfh.bfSize);
    Write(file, bfh.bfReserved1);
    Write(file, bfh.bfReserved2);
    Write(file, bfh.bfOffBits);

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

    vector<uint8_t> raw_image(bv5h.bV5SizeImage);
    int index = 0;
    for (int j = bv5h.bV5Height - 1; j >= 0; j--) {
        for (int i = 0; i < bv5h.bV5Width; i++) {
            raw_image[index + 0] = data(i, j).blue;
            raw_image[index + 1] = data(i, j).green;
            raw_image[index + 2] = data(i, j).red;
            raw_image[index + 3] = data(i, j).alpha;
            index += 4;
        }
    }
    file.write((char*)raw_image.data(), raw_image.size());
    file.close();
}

} // namespace utils
} // namespace prowogene
