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
TEST(ParseIntTest, ImageCreation) {
    image_utils::Image image0;
    EXPECT_EQ(0, image0.width());
    EXPECT_EQ(0, image0.height());
    EXPECT_EQ(0, image0.pixels().size());
    image_utils::Image image1(640, 480);
    EXPECT_EQ(640, image0.width());
    EXPECT_EQ(480, image0.height());
    EXPECT_EQ(640 * 480, image0.pixels().size());
}
}

