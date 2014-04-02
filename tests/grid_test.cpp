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
  std::cout << sg;
}
TEST(GridTest, ImageIntegralTest) {
  SummableGrid<int> sg(glm::ivec3(3, 3, 3));
  sg.Init();
  sg.AssignToAll(1);
  sg.IntegralImage();
  std::cout << sg;
}
TEST(GridTest, StepTest) {
  SummableGrid<int> sg(glm::ivec3(3, 3, 3));
  sg.Init();
  glm::ivec3 index(0);
  std::cout << sg.grid_size() << "\n";
  for(int i = 0; i < sg.grid_size(); ++i) {
    std::cout << index;
    if(index[0] == sg.size()[0] - 1) std::cout << "\n";
    if(index[0] == sg.size()[0] - 1 && index[1] == sg.size()[1] - 1) std::cout << "\n";
    index = sg.Step(index);
  }
  std::cout << "\n";
}
}

