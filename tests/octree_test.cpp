/*
 * raytracer_test.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: agrippa
 */

#include <climits>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "octree_base.hpp"
#include "octree.hpp"
namespace ray {
TEST(OctreeTest, NodeEncodeTest) {
  OctNode internal_node;
  OctNode test_node;
  EncodedNode encoded;
  OctNodeFactory& fact = OctNodeFactory::GetInstance();
  for (uint32_t i = 0; i < 8; ++i) {
    internal_node = fact.CreateInternal(i);
    internal_node.set_offset(42 * i);
    internal_node.set_size(i);
    encoded = fact.CreateEncodedNode(internal_node);
    test_node = fact.CreateOctNode(encoded);
    EXPECT_TRUE(internal_node.IsInternal());
    EXPECT_EQ(internal_node.type(), test_node.type());
    EXPECT_EQ(internal_node.octant(), test_node.octant());
    EXPECT_EQ(internal_node.offset(), test_node.offset());
    EXPECT_EQ(internal_node.size(), test_node.size());
  }
  for (uint32_t i = 0; i < 8; ++i) {
    internal_node = fact.CreateLeaf(i);
    internal_node.set_offset(1337 * i);
    internal_node.set_size(i);
    encoded = fact.CreateEncodedNode(internal_node);
    test_node = fact.CreateOctNode(encoded);
    EXPECT_TRUE(internal_node.IsLeaf());
    EXPECT_EQ(internal_node.type(), test_node.type());
    EXPECT_EQ(internal_node.octant(), test_node.octant());
    EXPECT_EQ(internal_node.offset(), test_node.offset());
    EXPECT_EQ(internal_node.size(), test_node.size());
  }
}
} // namespace ray

