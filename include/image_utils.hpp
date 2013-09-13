/*
 * image_utils.hpp
 *
 *  Created on: Sep 12, 2013
 *      Author: agrippa
 */

#ifndef IMAGE_UTILS_HPP_
#define IMAGE_UTILS_HPP_
namespace image_utils {
bool WritePNG(const char* file_name, const u_char* rgb_bytes, uint32_t width,
		uint32_t height);
}
#endif /* IMAGE_UTILS_HPP_ */
