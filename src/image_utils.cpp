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

bool ImageStorage::ReadImage(const std::string& file_name, Image& image,
        std::string& status) {
    bool success = true;
    Magick::Image image_gm;
    status = "OK";
    try {
        image_gm.read(file_name);
    } catch (Magick::WarningCoder &warning) {
        status = std::string(warning.what());
    } catch (Magick::Warning &warning) {
        status = std::string(warning.what());
    } catch (Magick::ErrorFileOpen &error) {
        success = false;
        status = std::string(error.what());
    } catch (Magick::Exception &error) {
        success = false;
        status = std::string(error.what());
    }
    return success;
}

bool ImageStorage::WriteImage(const std::string& file_name,
        const Image& image) {
    Magick::Image image_gm(image.width(), image.height(), "RGB",
            Magick::CharPixel, image.bytes());
    image_gm.write(file_name);
    return true;
}

ImageStorage& ImageStorage::GetInstance() {
    static ImageStorage instance;
    static bool is_initialized = false;
    if (!is_initialized) {
        char pwd[1024];
        getcwd(pwd, 1024);
        Magick::InitializeMagick(pwd);
        is_initialized = true;
    }
    return instance;
}
}
