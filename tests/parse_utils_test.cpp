/*
 * parse_utils_test.cpp
 *
 *  Created on: Sep 9, 2013
 *      Author: agrippa
 */

#include <climits>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "gtest/gtest.h"
#include "parse_utils.hpp"

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

TEST(ParseIntTest, EvenAndOdd) {
    int max_power = 30;  // log_2((INT_MAX + 1)/2)
    int min_power = 1;
    int even, odd, value;
    char buffer[256];
    bool result;
    for (int i = min_power; i <= max_power; ++i) {
        even = static_cast<int>(0x1 << i);
        odd = even - 1;
        snprintf(buffer, sizeof(buffer), "%d", even);
        //std::cout << even << " " << buffer << std::endl;
        result = ray::parse_utils::ParseInt(buffer, &value);
        EXPECT_EQ(even, value);
        EXPECT_TRUE(result);

        snprintf(buffer, sizeof(buffer), "%d", odd);
        result = ray::parse_utils::ParseInt(buffer, &value);
        EXPECT_EQ(odd, value);
        EXPECT_TRUE(result);
    }
}

TEST(ParseIntTest, NotAnInt) {
    int value;
    bool result;
    result = ray::parse_utils::ParseInt("not_an_int", &value);
    EXPECT_FALSE(result);
}

TEST(ParseIntTest, SpuriousNonDigit) {
    int value;
    bool result;
    result = ray::parse_utils::ParseInt("102d102", &value);
    EXPECT_FALSE(result);
}

TEST(ParseIntTest, LeadingZero) {
    int value;
    bool result;
    result = ray::parse_utils::ParseInt("01", &value);
    EXPECT_TRUE(result);
    EXPECT_EQ(1, value);
}

TEST(ParseIntTest, Empty) {
    int value;
    bool result;
    result = ray::parse_utils::ParseInt("", &value);
    EXPECT_FALSE(result);
}

TEST(ParseIntTest, Overflow) {
    int value;
    bool result;
    result = ray::parse_utils::ParseInt("11111111111111111111111111", &value);
    EXPECT_FALSE(result);
}

