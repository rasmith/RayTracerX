/*
 * image_storage_test.cpp
 *
 *  Created on: Sep 18, 2013
 *      Author: agrippa
 */

#include <climits>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <glm/glm.hpp>
#include "gtest/gtest.h"
#include "image_utils.hpp"

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
    u_char num_colors = 10;
    u_char color = 0;
    image.resize(640, 480);
    for (int i = 0; i < 480; ++i) {
        for (int j = 0; j < 640; ++j) {
            color = ((255u / num_colors) * i) % num_colors
            ;
            image(i, j) = glm::bvec3(color, color, color);
        }
    }
}
}

