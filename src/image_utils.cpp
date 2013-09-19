/*
 * image_utils.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: agrippa
 */
#include <GraphicsMagick/Magick++.h>
#include <glm/glm.hpp>
#include "image_utils.hpp"

namespace ray {
namespace image_utils {
Image::Image() :
        width_(0), height_(0) {
}

Image::Image(int width, int height) :
        width_(width), height_(height_) {
    pixels_.resize(width_ * height_);
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

glm::bvec3& Image::operator()(int i, int j) {
    return pixels_[i * width_ + j];
}

const std::vector<glm::bvec3>& Image::pixels() const {
    return pixels_;
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
        image_gm.type(TrueColorType);
        Magick::PixelPacket* pixels = image_gm.getPixels(0, 0,
                image_gm.columns(), image_gm.rows());
        Magick::ColorRGB color;
        int pos;
        for (int i = 0; i < image_gm.rows(); ++i) {
            for (int j = 0; j < image_gm.columns(); ++j) {
                pos = i * image_gm.columns() + j
                color = Magick::ColorRGB(
                        Magick::Color(pixels[pos], Magick::Color::RGBPixel));
                image(i, j) = static_cast<glm::bvec3>(255.0f
                        * glm::vec3(color.red(), color.green(), color.blue));
            }
        }
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

bool ImageStorage::WriteImage(const std::string& file_name, const Image& image,
        std::string& status) {
    bool success = true;
    Magick::Image image_gm(image.width(), image.height(), "RGB",
            Magick::CharPixel, image.bytes());
    status = "OK";
    try {
        image_gm.write(file_name);
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
}
