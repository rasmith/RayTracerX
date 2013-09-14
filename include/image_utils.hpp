/*
 * image_utils.hpp
 *
 *  Created on: Sep 12, 2013
 *      Author: agrippa
 */

#ifndef IMAGE_UTILS_HPP_
#define IMAGE_UTILS_HPP_
namespace image_utils {
class Image {
public:
	const std::vector<u_char>& bytes() const {
		return bytes_;
	}

	void set_bytes(const std::vector<u_char>& bytes) {
		bytes_ = bytes;
	}

	uint32_t height() const {
		return height_;
	}

	void set_height(uint32_t height) {
		height_ = height;
	}

	uint32_t width() const {
		return width_;
	}

	void set_width(uint32_t width) {
		width_ = width;
	}

private:
	std::vector<u_char> bytes_;
	uint32_t width_;
	uint32_t height_;
};

class ImageStorage {
public:
	static ImageStorage& GetInstance();
	~ImageStorage();
	bool ReadImage(const std::string& file_name, Image& image);
	bool WriteImage(const std::string& file_name, const Image& image);
private:
	ImageStorage();
	ImageStorage(const ImageStorage&);
};
}
#endif /* IMAGE_UTILS_HPP_ */
