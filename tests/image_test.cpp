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

TEST(ParseIntTest, ExtremeValues) {
    int value;
    bool result;

    // check 0
    result = ray::parse_utils::ParseInt("0", &value);
    EXPECT_EQ(0, value);

    // check INT_MAX
    int max_int = INT_MAX;
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%d", max_int);
    ray::parse_utils::ParseInt(buffer, &value);
    EXPECT_EQ(max_int, value);
    EXPECT_TRUE(result);
}


