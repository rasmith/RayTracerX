/*
 * image_utils.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: agrippa
 */
#include <iostream>
#include <vector>
#include <Magick++.h>
#include "image.hpp"
namespace ray {
Image::Image() :
        width_(0), height_(0) {
}

Image::Image(int width, int height) :
        width_(width), height_(height) {
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

void Image::resize(int width, int height) {
    set_width(width);
    set_height(height);
    resize();
}

void Image::resize() {
    pixels_.resize(width_ * height_);
}

ucvec3& Image::operator()(int i, int j) {
    return pixels_[i * width_ + j];
}

const std::vector<ucvec3>& Image::pixels() const {
    return pixels_;
}

ImageStorage::ImageStorage() {
}

ImageStorage::ImageStorage(const ImageStorage&) {
}

ImageStorage::~ImageStorage() {
}

bool ImageStorage::ReadImage(
        const std::string& file_name,
        Image& image,
        std::string& status) {
    bool success = true;
    ucvec3* pixels = NULL;
    Magick::Image image_gm;
    status = "OK";
    try {
        image_gm.read(file_name);
        image.resize(image_gm.columns(), image_gm.rows());
        pixels = &image(0, 0);
        image_gm.write(0, 0, image.width(), image.height(), "RGB",
                Magick::CharPixel, pixels);
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

bool ImageStorage::WriteImage(
        const std::string& file_name,
        const Image& image,
        std::string& status) {
    bool success = true;
    const std::vector<ucvec3>& pixels = image.pixels();
    Magick::Image image_gm;
    status = "OK";
    try {
        image_gm.read(image.width(), image.height(), "RGB", Magick::CharPixel,
                &pixels[0]);
        image_gm.write(file_name);
    } catch (Magick::WarningCoder &warning) {
        status = "Warning:WarningCoder:" + std::string(warning.what());
    } catch (Magick::Warning &warning) {
        status = "Warning:Warning" + std::string(warning.what());
    } catch (Magick::ErrorFileOpen &error) {
        success = false;
        status = "Error:ErrorFileOpen:" + std::string(error.what());
    } catch (Magick::Exception &error) {
        success = false;
        status = "Error:Exception:" + std::string(error.what());
    }
    return success;
}

ImageStorage& ImageStorage::GetInstance() {
    static ImageStorage instance;
    static bool is_initialized = false;
    if (!is_initialized) {
        char pwd[1024];
        if (NULL != getcwd(pwd, 1024)) {
            Magick::InitializeMagick(pwd);
            is_initialized = true;
        }
    }
    return instance;
}
} // namespace ray
