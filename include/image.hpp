/*
 * image.hpp
 *
 *  Created on: Sep 12, 2013
 *      Author: agrippa
 */
#ifndef IMAGE_HPP_
#define IMAGE_HPP_
#include <stdint.h>
#include <string>
#include <vector>
#include "types.hpp"
namespace ray {
class Image {
public:
    Image();
    Image(int width, int height);
    uint32_t height() const;
    void set_height(uint32_t height);
    uint32_t width() const;
    void set_width(uint32_t width);
    ucvec3& operator()(int i, int j);
    const std::vector<ucvec3>& pixels() const;
    void Resize(int width, int height);
    void Resize();
private:
    std::vector<ucvec3> pixels_;
    uint32_t width_;
    uint32_t height_;
};

class ImageStorage {
public:
    static ImageStorage& GetInstance();
    ~ImageStorage();
    bool ReadImage(const std::string& file_name, Image& image,
            std::string& status);
    bool WriteImage(const std::string& file_name, const Image& image,
            std::string& status);
private:
    ImageStorage();
    ImageStorage(const ImageStorage&);
};
} // namespace ray
#endif /* IMAGE_HPP_ */
