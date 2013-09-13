/*
 * image_utils.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: agrippa
 */
#include <png.h>
bool WritePNG(const char* file_name, const u_char* rgb_bytes, uint32_t width,
		uint32_t height) {
	bool success = false;
	FILE* file = fopen(file_name, "wb");
	if (!file)
		return false;
	png_structp write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
	NULL, NULL, NULL);
	if (!write_ptr)
		return false;
	png_infop info_ptr = png_create_info_struct(write_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&write_ptr, NULL);
	}
	return success;
}

