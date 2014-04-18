/*
 * image_storage_test.cpp
 *
 *  Created on: Sep 18, 2013
 *      Author: agrippa
 */

#include <climits>
#include <cstdlib>
#include <cstdio>
#include <stdint.h>
#include <string>
#include <glm/glm.hpp>
#include "gtest/gtest.h"
#include "image.hpp"
#include "types.hpp"
namespace ray {
TEST(ImageStorageTest, ImageReadTest) {
    ImageStorage& storage =
            ImageStorage::GetInstance();
    Image image;
    std::string path = "../assets/red.JPG";
    std::string status = "";
    bool success = storage.ReadImage(path, image, status);
    EXPECT_TRUE(success);
    EXPECT_EQ("OK", status);
    EXPECT_EQ(1024u, image.width());
    EXPECT_EQ(768u, image.height());
}
TEST(ImageStorageTest, ImageWriteTest) {
    ImageStorage& storage =
            ImageStorage::GetInstance();
    Image image;
    std::string path = "../assets/colors.jpg";
    std::string status = "";
    image.Resize(640, 480);
    for (int i = 0; i < 480; ++i) {
        for (int j = 0; j < 640; ++j) {
            image(i, j) = ucvec3(0u, 255u, 0u);
        }
    }
    bool successful_write = storage.WriteImage("../assets/colors.jpg", image,
            status);
    EXPECT_TRUE(successful_write);
}
TEST(ImageStorageTest, ImageReadWrite) {
    ImageStorage& storage =
            ImageStorage::GetInstance();
    Image image_src;

    std::string src_path = "../assets/checkerboard.jpg";
    std::string dest_path = "../assets/checkerboard_copy.jpg";
    std::string status = "";

    // load the checkerboard
    bool success = storage.ReadImage(src_path, image_src, status);
    EXPECT_TRUE(success);
    EXPECT_EQ("OK", status);

    // copy it to another image
    bool successful_write = storage.WriteImage(dest_path, image_src, status);
    EXPECT_EQ("OK", status);
    EXPECT_TRUE(successful_write);

    // read result back in
    Image image_dest;
    success = storage.ReadImage(dest_path, image_dest, status);
    EXPECT_EQ("OK", status);
    EXPECT_TRUE(success);
    EXPECT_EQ(image_dest.width(), image_src.width());
    EXPECT_EQ(image_dest.height(), image_src.height());
    float sum_error = 0;
    float error = 0;
    float size = static_cast<float>(image_dest.width() * image_dest.height());
    float thresh = size * 0.005;
    for (uint32_t i = 0; i < image_src.height(); ++i) {
        for (uint32_t j = 0; j < image_src.width(); ++j) {
            glm::vec3 pixel_src = static_cast<glm::vec3>(image_src(i, j));
            glm::vec3 pixel_dest = static_cast<glm::vec3>(image_dest(i, j));
            error = glm::distance(pixel_src, pixel_dest);
            sum_error += error;
        }
    }
    EXPECT_LT(error, thresh);
}
} // namespace ray

