#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {
    PACKED_STRUCT_BEGIN BitmapFileHeader {
        char sig1 = 'B';
        char sig2 = 'M';
        uint32_t size;
        uint32_t reserved = 0;
        uint32_t indent = 54;
    }
    PACKED_STRUCT_END

    PACKED_STRUCT_BEGIN BitmapInfoHeader {
        uint32_t size = 40;
        uint32_t width;
        uint32_t height;
        uint16_t planes = 1;
        uint16_t bits_per_pixel = 24;
        uint32_t compression = 0;
        uint32_t data_bytes;
        int32_t vertical_resolution = 11811;
        int32_t horizontal_resolution = 11811;
        int32_t used_colors = 0;
        int32_t colors = 0x1000000;
    }
    PACKED_STRUCT_END

    //calculate indent by width
    static int GetBMPStride(int w) {
        return 4 * ((w * 3 + 3) / 4);
    }

    // напишите эту функцию
    bool SaveBMP(const Path& file, const Image& image) {
        ofstream ofs(file, ios::binary);

        BitmapFileHeader file_header;

        ofs.write(&file_header.sig1, 1);
        ofs.write(&file_header.sig2, 1);
        file_header.size = 54 + GetBMPStride(image.GetWidth()) * image.GetHeight();
        ofs.write(reinterpret_cast<const char *>(&file_header.size), sizeof(file_header.size));
        ofs.write(reinterpret_cast<const char *>(&file_header.reserved), sizeof(file_header.reserved));
        ofs.write(reinterpret_cast<const char *>(&file_header.indent), sizeof(file_header.indent));

        BitmapInfoHeader info_header;
        ofs.write(reinterpret_cast<const char *>(&info_header.size), sizeof(info_header.size));
        info_header.width = image.GetWidth();
        info_header.height = image.GetHeight();
        ofs.write(reinterpret_cast<const char *>(&info_header.width), sizeof(info_header.width));
        ofs.write(reinterpret_cast<const char *>(&info_header.height), sizeof(info_header.height));
        ofs.write(reinterpret_cast<const char *>(&info_header.planes), sizeof(info_header.planes));
        ofs.write(reinterpret_cast<const char *>(&info_header.bits_per_pixel), sizeof(info_header.bits_per_pixel));
        ofs.write(reinterpret_cast<const char *>(&info_header.compression), sizeof(info_header.compression));
        info_header.data_bytes = GetBMPStride(image.GetWidth()) * image.GetHeight();
        ofs.write(reinterpret_cast<const char *>(&info_header.data_bytes), sizeof(info_header.data_bytes));
        ofs.write(reinterpret_cast<const char *>(&info_header.vertical_resolution), sizeof(info_header.vertical_resolution));
        ofs.write(reinterpret_cast<const char *>(&info_header.horizontal_resolution), sizeof(info_header.horizontal_resolution));
        ofs.write(reinterpret_cast<const char *>(&info_header.used_colors), sizeof(info_header.used_colors));
        ofs.write(reinterpret_cast<const char *>(&info_header.colors), sizeof(info_header.colors));

        for (int h = info_header.height - 1; h >= 0; --h) {
            const Color* line = image.GetLine(h);
            std::vector<char> buff(GetBMPStride(info_header.width));
            for (int w = 0; w < info_header.width; ++w) {
                buff[w * 3 + 0] = static_cast<char>(line[w].b);
                buff[w * 3 + 1] = static_cast<char>(line[w].g);
                buff[w * 3 + 2] = static_cast<char>(line[w].r);
            }
            ofs.write(buff.data(), buff.size());
        }

        return ofs.good();
    }


    // напишите эту функцию
    Image LoadBMP(const Path& file) {
        ifstream ifs(file, ios::binary);

        ifs.ignore(18);
        int width;
        int height;
        ifs.read(reinterpret_cast<char *>(&width), sizeof(width));
        ifs.read(reinterpret_cast<char *>(&height), sizeof(height));
        ifs.ignore(28);

        Image result{width, height, Color::Black()};
        const int bmp_stride = GetBMPStride(width);

        std::vector<char> buff(bmp_stride);
        for (int h = height - 1; h >= 0; --h) {
            Color* line = result.GetLine(h);
            ifs.read(buff.data(), bmp_stride);
            for (int w = 0; w < width; ++w) {
                line[w].b = static_cast<byte>(buff[w * 3 + 0]);
                line[w].g = static_cast<byte>(buff[w * 3 + 1]);
                line[w].r = static_cast<byte>(buff[w * 3 + 2]);
            }
        }

        return result;
    }
} // namespace img_lib
