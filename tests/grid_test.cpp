/*
 * grid_test.cpp
 *
 *  Created on: Apr 1, 2014
 *      Author: agrippa
 */

#include <climits>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
using ::testing::ElementsAre;
#include "grid.hpp"
#include "io_utils.hpp"
#include "light.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "sah_octnode.hpp"
#include "sah_octree.hpp"
#include "octree_base.hpp"
#include "raytracer.hpp"
#include "scene.hpp"
#include "scene_utils.hpp"
#include "transform.hpp"

namespace ray {
TEST(GridTest, InitializationTest) {
  SummableGrid<int> sg(glm::ivec3(3, 3, 3));
  sg.Init();
  sg.AssignToAll(1);
  for (int k = 0; k < 3; ++k) {
    for (int j = 0; j < 3; ++j) {
      for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(1, sg(i, j, k));
      }
    }
  }
}

TEST(GridTest, StepTest) {
  SummableGrid<int> sg(glm::ivec3(3, 3, 3));
  sg.Init();
  glm::ivec3 index(0);
  for (int k = 0; k < 3; ++k) {
    for (int j = 0; j < 3; ++j) {
      for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(glm::ivec3(i, j, k), index);
        index = sg.Step(index);
      }
    }
  }
}

TEST(GridTest, ImageIntegralTest) {
  SummableGrid<int> sg(glm::ivec3(3, 3, 3));
  SummableGrid<int> check(glm::ivec3(3, 3, 3));
  sg.Init();
  sg.AssignToAll(1);
  sg.ImageIntegral();
  check.Init();
  glm::ivec3 index(0);
  for (int n = 0; n < check.grid_size(); ++n) {
    int value = (index[0] + 1) * (index[1] + 1) * (index[2] + 1);
    check(index) = value;
    index = check.Step(index);
  }
  EXPECT_EQ(check, sg);
  if (check != sg) {
    std::cout << "sg = " << sg << std::endl;
    std::cout << "check = " << check << std::endl;
  }
}

TEST(GridTest, OrientedImageIntegralTest) {
  SummableGrid<int> sg(glm::ivec3(3, 3, 3));
  SummableGrid<int> check(glm::ivec3(3, 3, 3));
  sg.Init();
  check.Init();
  int dirs[2] = { 1, -1 };
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      for (int k = 0; k < 2; ++k) {
        glm::ivec3 o(dirs[i], dirs[j], dirs[k]);
        sg.AssignToAll(1);
        sg.OrientedImageIntegral(o);
        glm::ivec3 index(0);
        for (int n = 0; n < check.grid_size(); ++n) {
          int value = (index[0] + 1) * (index[1] + 1) * (index[2] + 1);
          check(check.OrientedIndex(index, o)) = value;
          index = check.Step(index);
        }
        EXPECT_EQ(check, sg);
        if (check != sg) {
          std::cout << "o = " << o << std::endl;
          std::cout << "sg = " << sg << std::endl;
          std::cout << "check = " << check << std::endl;
        }
      }
    }
  }
}
}

