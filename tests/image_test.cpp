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
    image_utils::Image image0;
    EXPECT_EQ(0u, image0.width());
    EXPECT_EQ(0u, image0.height());
    EXPECT_EQ(0u, image0.pixels().size());
    image_utils::Image image1(640, 480);
    EXPECT_EQ(640u, image1.width());
    EXPECT_EQ(480u, image1.height());
    EXPECT_EQ(640u * 480u, image1.pixels().size());
}
}

