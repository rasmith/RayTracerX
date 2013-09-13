/*
 * image_utils.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: agrippa
 */
#include <png.h>
namespace image_utils {
void WriteRowCallback(png_structp png_ptr, png_uint_32 row_number, int pass) {
	if (png_ptr == NULL || row_number > PNG_UINT_31_MAX || pass > 7)
		return;
}
bool WritePNG(const char* file_name, const u_char* rgb_bytes, uint32_t width,
		uint32_t height) {
	bool success = false;

	// open file for writing
	FILE* file = fopen(file_name, "wb");
	if (!file)
		return false;

	// setup write struct and info struct
	png_structp write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
	NULL, NULL, NULL);
	if (!write_ptr)
		return false;
	png_infop info_ptr = png_create_info_struct(write_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&write_ptr, NULL);
		return false;
	}

	// setup error handling
	if (setjmp(png_jmpbuf(write_ptr))) {
		png_destroy_info_struct(write_ptr, &info_ptr);
		png_destroy_write_struct(&write_ptr, &info_ptr);
		fclose(file);
		return false;
	}

	// setup output code
	png_init_io(write_ptr, file);

	// setup write callback
	png_set_write_status_fn(write_ptr, WriteRowCallback);

	// write using high-level write interface
	png_bytep* row_pointers = png_malloc(write_ptr, height * sizeof(png_bytep));
	for (int i = 0; i < height; ++i) {
		row_pointers[i] = static_cast<png_bytep>(rgb_bytes + i * width * 3);
	}
	png_set_rows(write_ptr, info_ptr, row_pointers);
	png_write_png(write_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	png_destroy_info_struct(write_ptr, &info_ptr);
	png_destroy_write_struct(&write_ptr, &info_ptr);
	fclose(file);
	return success;
}
}
