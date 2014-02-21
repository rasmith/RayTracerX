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

#include "camera.hpp"
#include "geometry.hpp"
#include "io_utils.hpp"
#include "light.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "octree.hpp"
#include "octree_base.hpp"
#include "raytracer.hpp"
#include "scene.hpp"
#include "scene_utils.hpp"
#include "transform.hpp"
template<typename T>
void PrintBinary(T c) {
  uint32_t num_bits = sizeof(T) * 8;
  for (uint32_t i = 0; i < num_bits; ++i) {
    if (0 == i % 8 && i > 0)
      std::cout << ' ';
    std::cout << (c & (0x1 << (num_bits - i - 1)) ? '1' : '0');

  }
}
namespace ray {
TEST(OctreeTest, NodeEncodeTest) {
  OctNode internal_node;
  OctNode test_node;
  EncodedNode encoded;
  OctNodeFactory& fact = OctNodeFactory::GetInstance();
  OctNode leaf_node = fact.CreateLeaf(0);
  leaf_node.set_offset(0);
  leaf_node.set_size(4968);
  std::cout << "Before --> ";
  PrintBinary(4968u);
  std::cout << "\n";
  leaf_node = fact.CreateOctNode(fact.CreateEncodedNode(leaf_node));
  std::cout << "After --> ";
  PrintBinary(leaf_node.size());
  std::cout << "\n";
  EXPECT_TRUE(leaf_node.IsLeaf());
  EXPECT_EQ(0u, leaf_node.octant());
  EXPECT_EQ(0u, leaf_node.offset());
  EXPECT_EQ(4968u, leaf_node.size());
  for (uint32_t i = 0; i < 8; ++i) {
    internal_node = fact.CreateInternal(i);
    internal_node.set_offset(42 * (0x1 << (4 * i)));
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
    internal_node.set_offset(1337 * (0x1 << (4 * i)));
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
TEST(OctreeTest, ChildBoundsTest) {
  BoundingBox bounds = BoundingBox(glm::vec3(0.0f), glm::vec3(1.0f));
  Octree<TrimeshFace> octree;
  BoundingBox bounds0 = octree.GetChildBounds(bounds, 0); // 000
  EXPECT_EQ(glm::vec3(0.0f, 0.0f, 0.0f), bounds0.min());
  EXPECT_EQ(glm::vec3(0.5f, 0.5f, 0.5f), bounds0.max());
  BoundingBox bounds1 = octree.GetChildBounds(bounds, 1); // 001
  EXPECT_EQ(glm::vec3(0.5f, 0.0f, 0.0f), bounds1.min());
  EXPECT_EQ(glm::vec3(1.0f, 0.5f, 0.5f), bounds1.max());
  BoundingBox bounds2 = octree.GetChildBounds(bounds, 2); // 010
  EXPECT_EQ(glm::vec3(0.0f, 0.5f, 0.0f), bounds2.min());
  EXPECT_EQ(glm::vec3(0.5f, 1.0f, 0.5f), bounds2.max());
  BoundingBox bounds3 = octree.GetChildBounds(bounds, 3); // 011
  EXPECT_EQ(glm::vec3(0.5f, 0.5f, 0.0f), bounds3.min());
  EXPECT_EQ(glm::vec3(1.0f, 1.0f, 0.5f), bounds3.max());
  BoundingBox bounds4 = octree.GetChildBounds(bounds, 4); // 100
  EXPECT_EQ(glm::vec3(0.0f, 0.0f, 0.5f), bounds4.min());
  EXPECT_EQ(glm::vec3(0.5f, 0.5f, 1.0f), bounds4.max());
  BoundingBox bounds5 = octree.GetChildBounds(bounds, 5); // 101
  EXPECT_EQ(glm::vec3(0.5f, 0.0f, 0.5f), bounds5.min());
  EXPECT_EQ(glm::vec3(1.0f, 0.5f, 1.0f), bounds5.max());
  BoundingBox bounds6 = octree.GetChildBounds(bounds, 6); // 110
  EXPECT_EQ(glm::vec3(0.0f, 0.5f, 0.5f), bounds6.min());
  EXPECT_EQ(glm::vec3(0.5f, 1.0f, 1.0f), bounds6.max());
  BoundingBox bounds7 = octree.GetChildBounds(bounds, 7); // 111
  EXPECT_EQ(glm::vec3(0.5f, 0.5f, 0.5f), bounds7.min());
  EXPECT_EQ(glm::vec3(1.0f, 1.0f, 1.0f), bounds7.max());
}
TEST(RayTracerTest, BunnyMeshTest) {
  SceneLoader& loader = SceneLoader::GetInstance();
  std::string path = "../assets/bunny.obj";
  std::string status = "";
  Scene scene;
  std::cout << "loading" << std::endl;
  bool success = loader.LoadScene(path, scene, status);
  EXPECT_TRUE(success);
  EXPECT_EQ("OK", status);
  int image_width = 512;
  int image_height = 512;
  glm::vec3 eye_pos = glm::vec3(0.0f, 0.1f, 0.5f);
  glm::vec3 at_pos = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 up_dir = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::mat4x4 look_at = LookAt(eye_pos, at_pos, up_dir);
  Camera camera(image_width, image_height, Orthographic(0.0f, 1.0f), look_at);

  Light point_light;
  glm::vec3 point_light_color = glm::vec3(1.0f, 0.3f, 0.3f);
  point_light.ka = point_light_color;
  point_light.kd = point_light_color;
  point_light.ks = point_light_color;
  point_light.ray = Ray(glm::vec3(0.0, 1.0, 2.0f), glm::vec3(0.0f));
  point_light.type = Light::kPoint;
  point_light.attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
      0.000045492f);

  Light directional_light;
  glm::vec3 directional_light_color = glm::vec3(0.2f, 0.2f, 0.2f);
  directional_light.ka = directional_light_color;
  directional_light.kd = directional_light_color;
  directional_light.ks = directional_light_color;
  directional_light.ray = Ray(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  directional_light.type = Light::kDirectional;

  scene.AddLight(point_light);
  scene.AddLight(directional_light);

  Trimesh* trimesh = static_cast<Trimesh*>(scene.scene_objects()[0]);
  Octree<TrimeshFace> octree;
  std::cout << "Building octree" << std::endl;
  octree.Build(trimesh->faces());
  std::cout << "Octree built.";
  trimesh->set_accelerator(&octree);

  Image image;
  image.resize(image_width, image_height);
  RayTracer ray_tracer(&scene, &camera);

  ray_tracer.Render(image);
  ImageStorage& storage = ImageStorage::GetInstance();
  success = storage.WriteImage("bunny_octree.jpg", image, status);
  EXPECT_TRUE(success);
  EXPECT_EQ("OK", status);
}
} // namespace ray

