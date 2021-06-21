#include <exception>
#include <iostream>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include <png.h>

#include "stereogram.hpp"

int width, height;
png_byte color_type;
png_byte bit_depth;
png_structp png_ptr;
png_infop info_ptr;
int number_of_passes;
png_bytep* row_pointers;
png_bytep bitmap;

void read_png_file(char* file_name)
{
    unsigned char header[8];
    FILE *fp = fopen(file_name, "rb");
    if (!fp)
    {
        throw std::runtime_error("File could not be opened for reading");
    }
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
    {
        throw std::runtime_error("File is not recognized as a PNG file");
    }
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        throw std::runtime_error("png_create_read_struct failed");
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        throw std::runtime_error("png_create_info_struct failed");
    }
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        throw std::runtime_error("Error during init_io");
    }
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    number_of_passes = png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        throw std::runtime_error("Error during read_image");
    }
    if (height * png_get_rowbytes(png_ptr,info_ptr) > 0x4000000)
    {
        throw std::runtime_error("image is too big.");
    }
    bitmap = (png_byte*) malloc(height * png_get_rowbytes(png_ptr,info_ptr));
    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
    for (int y=0; y<height; y++)
        row_pointers[y] = bitmap + y * png_get_rowbytes(png_ptr,info_ptr);
    png_read_image(png_ptr, row_pointers);
    fclose(fp);
}

void write_png_file(char* file_name)
{
    FILE *fp = fopen(file_name, "wb");
    if (!fp)
    {
        throw std::runtime_error("File %s could not be opened for writing");
    }
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        throw std::runtime_error("png_create_write_struct failed");
    }
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        throw std::runtime_error("png_create_info_struct failed");
    }
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        throw std::runtime_error("Error during init_io");
    }
    png_init_io(png_ptr, fp);
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        throw std::runtime_error("Error during writing header");
    }
    png_set_IHDR(png_ptr, info_ptr, width, height,
             bit_depth, color_type, PNG_INTERLACE_NONE,
             PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png_ptr, info_ptr);
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        throw std::runtime_error("Error during writing bytes");
    }
    png_write_image(png_ptr, row_pointers);
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        throw std::runtime_error("Error during end of write");
    }
    png_write_end(png_ptr, NULL);
    free(row_pointers);
    free(bitmap);
    fclose(fp);
}

void process_file(void)
{
    int column = stereogram::GetColumn(width);
    std::vector<unsigned char> pattern;
    switch (png_get_color_type(png_ptr, info_ptr))
    {
    case PNG_COLOR_TYPE_RGB:
        pattern = stereogram::CreatePattern<3>(column, 0x00010203);
        stereogram::Convert<3, 8>(bitmap, column, width, height, pattern.data());
        break;
    case PNG_COLOR_TYPE_RGBA:
        pattern = stereogram::CreatePattern<4>(column, 0x00010203);
        stereogram::Convert<4, 8>(bitmap, column, width, height, pattern.data());
        break;
    default:
        throw std::runtime_error("Unknown color type");
        break;
    }
}

int main(int argc, char **argv)
{
    if (argc == 2 && std::strcmp("--version", argv[1]) == 0)
    {
        std::cout << "Depth: " << PROJECT_VERSION << std::endl;
        return 0;
    }
    try
    {
        if (argc != 3)
        {
            throw std::runtime_error(
                "Usage: depth <in_file.png> <out_file.png>");
        }
        read_png_file(argv[1]);
        process_file();
        write_png_file(argv[2]);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
