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
}



