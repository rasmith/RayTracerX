/*
 * image_utils.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: agrippa
 */
#include <GraphicsMagick/Magick++.h>
namespace image_utils {
const std::vector<u_char>& Image::bytes() const {
    return bytes_;
}

void Image::set_bytes(const std::vector<u_char>& bytes) {
    bytes_ = bytes;
}

uint32_t Image::height() const {
    return height_;
}

void Image::set_height(uint32_t height) {
    height_ = height;
}

uint32_t Image::width() const {
    return width_;
}

void Image::set_width(uint32_t width) {
    width_ = width;
}

ImageStorage::ImageStorage() {
}

ImageStorage::ImageStorage(const ImageStorage&) {
}

ImageStorage::~ImageStorage() {

}

bool ImageStorage::ReadImage(const std::string& file_name, Image& image) {
    return false;
}

bool ImageStorage::WriteImage(const std::string& file_name,
        const Image& image) {
    return false;
}

ImageStorage& ImageStorage::GetInstance() {
    static ImageStorage instance;
    return instance;
}
}
