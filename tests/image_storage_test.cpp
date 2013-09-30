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
#include "image_utils.hpp"
#include "types.hpp"

namespace ray {
TEST(ImageStorageTest, ImageReadTest) {
    image_utils::ImageStorage& storage =
            image_utils::ImageStorage::GetInstance();
    image_utils::Image image;
    std::string path = "../assets/red.JPG";
    std::string status = "";
    bool success = storage.ReadImage(path, image, status);
    EXPECT_TRUE(success);
    EXPECT_EQ("OK", status);
    EXPECT_EQ(1024u, image.width());
    EXPECT_EQ(768u, image.height());
}
TEST(ImageStorageTest, ImageWriteTest) {
    image_utils::ImageStorage& storage =
            image_utils::ImageStorage::GetInstance();
    image_utils::Image image;
    std::string path = "../assets/colors.jpg";
    std::string status = "";
    image.resize(640, 480);
    for (int i = 0; i < 480; ++i) {
        for (int j = 0; j < 640; ++j) {
            image(i, j) = ucvec3(0u, 255u, 0u);
        }
    }
    bool successful_write = storage.WriteImage("../assets/colors.jpg", image,
            status);
    std::cout << "STATUS=" << status << std::endl;
    EXPECT_TRUE(successful_write);
}
TEST(ImageStorageTest, ImageReadWrite) {
    image_utils::ImageStorage& storage =
            image_utils::ImageStorage::GetInstance();
    image_utils::Image image;
    std::string src_path = "../assets/checkerboard.jpg";
    std::string dest_path = "../assets/checkerboard_copy.jpg";
    std::string status = "";
    bool success = storage.ReadImage(src_path, image, status);
    EXPECT_TRUE(success);
    EXPECT_EQ("OK", status);
    bool successful_write = storage.WriteImage(dest_path, image, status);
    EXPECT_EQ("OK", status);
    EXPECT_TRUE(successful_write);
}
}

