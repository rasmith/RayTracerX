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
TEST(GridTest, GridInitializationTest) {
  SummableGrid<int> sg;
  sg.AssignToAll(1);
  std::cout << sg;
}
}

