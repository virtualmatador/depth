#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <png.h>


void abort_(const char * s, ...)
{
        va_list args;
        va_start(args, s);
        vfprintf(stderr, s, args);	
        fprintf(stderr, "\n");
        va_end(args);
        abort();
}

int width, height;
png_byte color_type;
png_byte bit_depth;

png_structp png_ptr;
png_infop info_ptr;
int number_of_passes;
png_bytep * row_pointers;

void read_png_file(char* file_name)
{
        unsigned char header[8];    // 8 is the maximum size that can be checked

        /* open file and test for it being a png */
        FILE *fp = fopen(file_name, "rb");
        if (!fp)
                abort_("File %s could not be opened for reading", file_name);
        fread(header, 1, 8, fp);
        if (png_sig_cmp(header, 0, 8))
                abort_("File %s is not recognized as a PNG file", file_name);


        /* initialize stuff */
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
                abort_("png_create_read_struct failed");

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                abort_("png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("Error during init_io");

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);

        png_read_info(png_ptr, info_ptr);

        width = png_get_image_width(png_ptr, info_ptr);
        height = png_get_image_height(png_ptr, info_ptr);
        color_type = png_get_color_type(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);

        number_of_passes = png_set_interlace_handling(png_ptr);
        png_read_update_info(png_ptr, info_ptr);


        /* read file */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("Error during read_image");

        row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
        for (int y=0; y<height; y++)
                row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

        png_read_image(png_ptr, row_pointers);

        fclose(fp);
}


void write_png_file(char* file_name)
{
        /* create file */
        FILE *fp = fopen(file_name, "wb");
        if (!fp)
                abort_("[write_png_file] File %s could not be opened for writing", file_name);


        /* initialize stuff */
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
                abort_("[write_png_file] png_create_write_struct failed");

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                abort_("[write_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during init_io");

        png_init_io(png_ptr, fp);


        /* write header */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing header");

        png_set_IHDR(png_ptr, info_ptr, width, height,
                     bit_depth, color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr, info_ptr);


        /* write bytes */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing bytes");

        png_write_image(png_ptr, row_pointers);


        /* end write */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during end of write");

        png_write_end(png_ptr, NULL);

        /* cleanup heap allocation */
        for (int y=0; y<height; y++)
                free(row_pointers[y]);
        free(row_pointers);

        fclose(fp);
}


void process_file(void)
{
        if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGB)
                abort_("color_type of input file must be PNG_COLOR_TYPE_RGB (%d) (is %d)",
                       PNG_COLOR_TYPE_RGB, png_get_color_type(png_ptr, info_ptr));

	int iColumn = width / 100 * 10;
	int * arShift = new int[width];
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
			arShift[x] = 0;
                png_byte* row = row_pointers[y];
		int iSide = (y % 2) * 2 - 1;
		for (int x = width / 2 - iSide * width / 2 - (1 - iSide) / 2 + iSide * iColumn;
			iSide * x <= iSide * (width / 2 + iSide * width / 2 - (1 + iSide) / 2 - iSide * iColumn);
			x += iSide)
		{
			png_byte* ptr = &(row[x * 3]);
			int h = (ptr[0] + ptr[1] + ptr[2]) * 16 / 3 / 256;
			if (h != 0)
			{
				for (int k = x + iSide * iColumn / 2 - iSide * (h + h % 2 * (iSide + 1) / 2) / 2;
					iSide * k <= iSide * (width / 2 + iSide * width / 2 - (1 + iSide) / 2); k += iSide * iColumn)
				{
					arShift[k] = iSide * h;
					int iTarget = k + iSide * h;
					if (iTarget < 0)
						iTarget += iColumn;
					else if (iTarget >= width)
						iTarget -= iColumn;
					arShift[k] += arShift[iTarget];
					if (arShift[k] < 0)
						arShift[k] += iColumn;
					else if (arShift[k] >= iColumn)
						arShift[k] -= iColumn;
				}
			}
		}

		for (int x = 0; x < width; ++x)
		{
			png_byte* ptr = &(row[x * 3]);
			double yy = y % iColumn;
			double xx = (x + arShift[x]) % iColumn;
			ptr[0] = png_byte(int(pow(yy + iColumn / 3.6, 1.45) * pow(xx + iColumn / 3.4, 1.55)) % 256);
			ptr[1] = png_byte(int(pow(yy + iColumn / 4.8, 1.15) * pow(xx + iColumn / 4.6, 1.25)) % 256);
			ptr[2] = png_byte(int(pow(yy + iColumn / 2.2, 1.85) * pow(xx + iColumn / 2.4, 1.75)) % 256);
		}
	}
	delete[] arShift;
}


int main(int argc, char **argv)
{
        if (argc != 3)
                abort_("Usage: depth <in_file.png> <out_file.png>");

        read_png_file(argv[1]);
        process_file();
        write_png_file(argv[2]);

        return 0;
}

