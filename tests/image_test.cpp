/*
 * image_test.cpp

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
TEST(ImageTest, ImageCreation) {
    image::Image image0;
    EXPECT_EQ(0u, image0.width());
    EXPECT_EQ(0u, image0.height());
    EXPECT_EQ(0u, image0.pixels().size());
    image::Image image1(640, 480);
    EXPECT_EQ(640u, image1.width());
    EXPECT_EQ(480u, image1.height());
    EXPECT_EQ(640u * 480u, image1.pixels().size());
}
TEST(ImageTest, ImageModify) {
    image::Image image(640, 480);
    EXPECT_EQ(640u, image.width());
    EXPECT_EQ(480u, image.height());
    EXPECT_EQ(640u * 480u, image.pixels().size());
    for (int i = 0; i < 480; ++i) {
        for (int j = 0; j < 640; ++j) {
            image(i,j) = ucvec3(0u, 255u, 0u);
            EXPECT_EQ(ucvec3(0, 255u, 0u), image(i, j));
            EXPECT_NE(ucvec3(0u, 0u, 0u), image(i, j));
        }
    }
}
}

