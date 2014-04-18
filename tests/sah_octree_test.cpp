/*
 * sah_octree_test.cpp
 *
 *  Created on: Mar 26, 2014
 *      Author: agrippa
 */

#include <sys/time.h>

#include <climits>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
using ::testing::ElementsAre;
#include "camera.hpp"
#include "geometry.hpp"
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

bool use_timing = true;
int num_timings = 10;
typedef SAHOctree<TrimeshFace> TestOctree;

TEST(OctreeTest, NodeEncodeTest) {
  SAHOctNode internal_node;
  SAHOctNode test_node;
  SAHEncodedNode encoded;
  SAHOctNodeFactory& fact = SAHOctNodeFactory::GetInstance();
  SAHOctNode leaf_node = fact.CreateLeaf(0);
  leaf_node.set_offset(0);
  leaf_node.set_size(4968);
  //PrintBinary(4968u);
  leaf_node = fact.CreateOctNode(fact.CreateEncodedNode(leaf_node));
  //PrintBinary(leaf_node.size());
  EXPECT_TRUE(leaf_node.IsLeaf());
  EXPECT_EQ(0u, leaf_node.octant());
  EXPECT_EQ(0u, leaf_node.offset());
  EXPECT_EQ(4968u, leaf_node.size());
  for (uint32_t i = 0; i < 8; ++i) {
    int k = static_cast<int>(i);
    internal_node = fact.CreateInternal(i);
    internal_node.set_offset(42 * (0x1 << (4 * i)));
    internal_node.set_size(i);
    internal_node.set_point(
        glm::vec3(1.11f * (2048 - k), 2.22f * (2048 - k), 3.33f * (2048 - k)));
    encoded = fact.CreateEncodedNode(internal_node);
    test_node = fact.CreateOctNode(encoded);
    EXPECT_TRUE(internal_node.IsInternal());
    EXPECT_EQ(internal_node.type(), test_node.type());
    EXPECT_EQ(internal_node.octant(), test_node.octant());
    EXPECT_EQ(internal_node.offset(), test_node.offset());
    EXPECT_EQ(internal_node.size(), test_node.size());
    if (!(internal_node.point() == test_node.point())) {
      std::cout << "internal_node.point() = " << internal_node.point()
          << std::endl;
      std::cout << "test_node.point() = " << test_node.point() << std::endl;
    }
    EXPECT_EQ(internal_node.point(), test_node.point());
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

TEST(OctreeTest, OrientationToOctantTest) {
  glm::ivec3 values[8] = { glm::ivec3(1, 1, 1), glm::ivec3(-1, 1, 1),
      glm::ivec3(1, -1, 1), glm::ivec3(-1, -1, 1), glm::ivec3(1, 1, -1),
      glm::ivec3(-1, 1, -1), glm::ivec3(1, -1, -1), glm::ivec3(-1, -1, -1) };
  TestOctree octree;
  for (uint32_t octant = 0; octant < 8; ++octant) {
    if (!(values[octant] == octree.OctantToOrientation(octant)))
      std::cout << octant << "-->" << octree.OctantToOrientation(octant)
          << " should be " << values[octant] << std::endl;
    EXPECT_EQ(values[octant], octree.OctantToOrientation(octant));
  }
}

TEST(OctreeTest, OctantToOrientationTest) {
  glm::ivec3 values[8] = { glm::ivec3(1, 1, 1), glm::ivec3(-1, 1, 1),
      glm::ivec3(1, -1, 1), glm::ivec3(-1, -1, 1), glm::ivec3(1, 1, -1),
      glm::ivec3(-1, 1, -1), glm::ivec3(1, -1, -1), glm::ivec3(-1, -1, -1) };
  TestOctree octree;
  glm::ivec3 orientation = glm::ivec3(0);
  for (uint32_t octant = 0; octant < 8; ++octant) {
    orientation = values[octant];
    if (!(octant == octree.OrientationToOctant(orientation)))
      std::cout << orientation << "-->"
          << octree.OrientationToOctant(orientation) << " should be " << octant
          << std::endl;
    EXPECT_EQ(octant, octree.OrientationToOctant(orientation));
  }
}

TEST(RayTracerTest, SphereMeshTest) {
  SceneLoader& loader = SceneLoader::GetInstance();
  std::string path = "../assets/sphere.obj";
  std::string status = "";
  Scene scene;
  std::cout << "loading" << std::endl;
  bool success = loader.LoadScene(path, scene, status);
  EXPECT_TRUE(success);
  EXPECT_EQ("OK", status);
  int image_width = 512;
  int image_height = 512;
  glm::vec3 eye_pos = glm::vec3(0.0f, 0.0f, 1.2f);
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
  directional_light.ray = Ray(glm::vec3(0.0f), glm::vec3(0.05f, 0.05f, 0.05f));
  directional_light.type = Light::kDirectional;

  scene.AddLight(point_light);
  scene.AddLight(directional_light);

  Trimesh* trimesh = static_cast<Trimesh*>(scene.scene_objects()[0]);
  TestOctree octree;
  std::cout << "Building octree" << std::endl;
  octree.Build(trimesh->faces());
  //octree.Print(std::cout);
  std::cout << "Octree built.\n";
  trimesh->set_accelerator(&octree);

  Image image;
  image.resize(image_width, image_height);
  RayTracer ray_tracer(&scene, &camera);
  ray_tracer.set_display_progress(!use_timing);
  ray_tracer.set_display_stats(!use_timing);
  ray_tracer.set_display_stats(!use_timing);
  ray_tracer.set_background_color(glm::vec3(0.05f, 0.05f, 0.05f));

  timeval t_start, t_finish;
  if (use_timing)
    gettimeofday(&t_start, NULL);
  std::cout << "Rendering..." << std::endl;
  ray_tracer.Render(image);
  if (use_timing) {
    gettimeofday(&t_finish, NULL);
    float sec = t_finish.tv_sec - t_start.tv_sec + t_finish.tv_usec / 1000000.0f
        - t_start.tv_usec / 1000000.0f;
    std::cout << "Render time:" << sec << std::endl;
  } else
    std::cout << "Done." << std::endl;

  ImageStorage& storage = ImageStorage::GetInstance();
  success = storage.WriteImage("sphere_octree.jpg", image, status);
  EXPECT_TRUE(success);
  EXPECT_EQ("OK", status);
}

TEST(RayTracerTest, BunnyMeshTest) {
  SceneLoader& loader = SceneLoader::GetInstance();
  std::string path = "../assets/bunny.ply";
  std::string status = "";
  Scene scene;
  std::cout << "loading" << std::endl;
  bool success = loader.LoadScene(path, scene, status);
  EXPECT_TRUE(success);
  EXPECT_EQ("OK", status);
  int image_width = 512;
  int image_height = 512;
  //[-0.0168008,0.110153,-0.00148227]
  glm::vec3 eye_pos = glm::vec3(-0.0168008f, 0.110153f, 0.225f);
  glm::vec3 at_pos = glm::vec3(-0.0168008f, 0.110153f, -0.00148227f);
  glm::vec3 up_dir = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::mat4x4 look_at = LookAt(eye_pos, at_pos, up_dir);
  Camera camera(image_width, image_height, Orthographic(0.0f, 1.0f), look_at);

  Light point_light;
  glm::vec3 point_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  point_light.ka = point_light_color;
  point_light.kd = point_light_color;
  point_light.ks = point_light_color;
  point_light.ray = Ray(glm::vec3(0.0, 1.0, 2.0f), glm::vec3(0.0f));
  point_light.type = Light::kPoint;
  point_light.attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
      0.000045492f);

  Light directional_light;
  glm::vec3 directional_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  directional_light.ka = directional_light_color;
  directional_light.kd = directional_light_color;
  directional_light.ks = directional_light_color;
  directional_light.ray = Ray(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  directional_light.type = Light::kDirectional;

  scene.AddLight(point_light);
  scene.AddLight(directional_light);

  Trimesh* trimesh = static_cast<Trimesh*>(scene.scene_objects()[0]);
  TestOctree octree;
  std::cout << "Building octree" << std::endl;
  octree.Build(trimesh->faces());
  std::cout << "Octree built.\n";
  std::cout << "bounds = " << octree.GetBounds() << " center = "
      << octree.GetBounds().GetCenter() << std::endl;
  trimesh->set_accelerator(&octree);
  //octree.Print(std::cout);
  Image image;
  image.resize(image_width, image_height);
  RayTracer ray_tracer(&scene, &camera);
  ray_tracer.set_display_progress(!use_timing);
  ray_tracer.set_display_stats(!use_timing);
  ray_tracer.set_background_color(glm::vec3(0.05f, 0.05f, 0.05f));

  std::cout << "Rendering..." << std::endl;
  timeval t_start, t_finish;
  float sum_sec = 0.0f;
  if (use_timing) {
    for (int t = 0; t < num_timings; ++t) {
      gettimeofday(&t_start, NULL);
      ray_tracer.Render(image);
      gettimeofday(&t_finish, NULL);
      float sec = t_finish.tv_sec - t_start.tv_sec
          + t_finish.tv_usec / 1000000.0f - t_start.tv_usec / 1000000.0f;
      sum_sec += sec;
      std::cout << "Render time:" << sec << std::endl;
    }
    std::cout << "Average render time:" << sum_sec / num_timings << std::endl;
  } else {
    ray_tracer.Render(image);
    std::cout << "Done." << std::endl;
  }

  ImageStorage& storage = ImageStorage::GetInstance();
  success = storage.WriteImage("bunny_octree.jpg", image, status);
  EXPECT_TRUE(success);
  EXPECT_EQ("OK", status);
}

TEST(RayTracerTest, DragonMeshTest) {
  SceneLoader& loader = SceneLoader::GetInstance();
  std::string path = "../assets/dragon.ply";
  std::string status = "";
  Scene scene;
  std::cout << "loading" << std::endl;
  bool success = loader.LoadScene(path, scene, status);
  EXPECT_TRUE(success);
  EXPECT_EQ("OK", status);
  std::cout << "done loading" << std::endl;
  int image_width = 512;
  int image_height = 512;
  glm::vec3 eye_pos = glm::vec3(-0.0058789f, 0.124951f, 0.275f);
  glm::vec3 at_pos = glm::vec3(-0.0058789f, 0.124951f, -0.0046034f);
  glm::vec3 up_dir = glm::vec3(0.0f, 1.0f, 0.0f);
  std::cout << "set camera" << std::endl;
  glm::mat4x4 look_at = LookAt(eye_pos, at_pos, up_dir);
  Camera camera(image_width, image_height, Orthographic(0.0f, 1.0f), look_at);
  std::cout << "camera set" << std::endl;

  Light point_light;
  glm::vec3 point_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  point_light.ka = point_light_color;
  point_light.kd = point_light_color;
  point_light.ks = point_light_color;
  point_light.ray = Ray(glm::vec3(0.0, -1.0, 2.0f), glm::vec3(0.0f));
  point_light.type = Light::kPoint;
  point_light.attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
      0.000045492f);

  Light directional_light;
  glm::vec3 directional_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  directional_light.ka = directional_light_color;
  directional_light.kd = directional_light_color;
  directional_light.ks = directional_light_color;
  directional_light.ray = Ray(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  directional_light.type = Light::kDirectional;

  scene.AddLight(point_light);
  scene.AddLight(directional_light);

  Trimesh* trimesh = static_cast<Trimesh*>(scene.scene_objects()[0]);
  TestOctree octree;
  std::cout << "Building octree" << std::endl;
  octree.Build(trimesh->faces());
  //octree.set_trace(true);
  //octree.Print(std::cout);
  std::cout << "Octree built.\n";
  std::cout << "bounds = " << octree.GetBounds() << " center = "
      << octree.GetBounds().GetCenter() << std::endl;
  trimesh->set_accelerator(&octree);
  Image image;
  image.resize(image_width, image_height);
  RayTracer ray_tracer(&scene, &camera);
  ray_tracer.set_display_progress(!use_timing);
  ray_tracer.set_display_stats(!use_timing);
  ray_tracer.set_background_color(glm::vec3(0.05f, 0.05f, 0.05f));

  timeval t_start, t_finish;
  if (use_timing)
    gettimeofday(&t_start, NULL);
  std::cout << "Rendering..." << std::endl;
  ray_tracer.Render(image);
  if (use_timing) {
    gettimeofday(&t_finish, NULL);
    float sec = t_finish.tv_sec - t_start.tv_sec + t_finish.tv_usec / 1000000.0f
        - t_start.tv_usec / 1000000.0f;
    std::cout << "Render time:" << sec << std::endl;
  } else
    std::cout << "Done." << std::endl;

  ImageStorage& storage = ImageStorage::GetInstance();
  success = storage.WriteImage("dragon_octree.jpg", image, status);
  EXPECT_TRUE(success);
  EXPECT_EQ("OK", status);
}

TEST(RayTracerTest, BuddhaMeshTest) {
  SceneLoader& loader = SceneLoader::GetInstance();
  std::string path = "../assets/happy.ply";
  std::string status = "";
  Scene scene;
  std::cout << "loading" << std::endl;
  bool success = loader.LoadScene(path, scene, status);
  EXPECT_TRUE(success);
  EXPECT_EQ("OK", status);
  std::cout << "done loading" << std::endl;
  int image_width = 512;
  int image_height = 512;
  glm::vec3 eye_pos = glm::vec3(-0.0054393f, 0.14769f, 0.275f);
  glm::vec3 at_pos = glm::vec3(-0.0054393f, 0.148769, -0.00669f);
  glm::vec3 up_dir = glm::vec3(0.0f, 1.0f, 0.0f);
  std::cout << "set camera" << std::endl;
  glm::mat4x4 look_at = LookAt(eye_pos, at_pos, up_dir);
  Camera camera(image_width, image_height, Orthographic(0.0f, 1.0f), look_at);
  std::cout << "camera set" << std::endl;

  Light point_light;
  glm::vec3 point_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  point_light.ka = point_light_color;
  point_light.kd = point_light_color;
  point_light.ks = point_light_color;
  point_light.ray = Ray(glm::vec3(0.0, -2.0, 2.0f), glm::vec3(0.0f));
  point_light.type = Light::kPoint;
  point_light.attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
      0.000045492f);

  Light directional_light;
  glm::vec3 directional_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  directional_light.ka = directional_light_color;
  directional_light.kd = directional_light_color;
  directional_light.ks = directional_light_color;
  directional_light.ray = Ray(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 1.0f));
  directional_light.type = Light::kDirectional;

  scene.AddLight(point_light);
  scene.AddLight(directional_light);

  Trimesh* trimesh = static_cast<Trimesh*>(scene.scene_objects()[0]);
  TestOctree octree;
  std::cout << "Building octree" << std::endl;
  octree.Build(trimesh->faces());
  //octree.set_trace(true);
  //octree.Print(std::cout);
  std::cout << "Octree built.\n";
  std::cout << "bounds = " << octree.GetBounds() << " center = "
      << octree.GetBounds().GetCenter() << std::endl;
  trimesh->set_accelerator(&octree);
  Image image;
  image.resize(image_width, image_height);
  RayTracer ray_tracer(&scene, &camera);
  ray_tracer.set_display_progress(!use_timing);
  ray_tracer.set_display_stats(!use_timing);
  ray_tracer.set_background_color(glm::vec3(0.05f, 0.05f, 0.05f));

  timeval t_start, t_finish;
  if (use_timing)
    gettimeofday(&t_start, NULL);
  std::cout << "Rendering..." << std::endl;
  ray_tracer.Render(image);
  if (use_timing) {
    gettimeofday(&t_finish, NULL);
    float sec = t_finish.tv_sec - t_start.tv_sec + t_finish.tv_usec / 1000000.0f
        - t_start.tv_usec / 1000000.0f;
    std::cout << "Render time:" << sec << std::endl;
  } else
    std::cout << "Done." << std::endl;

  ImageStorage& storage = ImageStorage::GetInstance();
  success = storage.WriteImage("buddha_octree.jpg", image, status);
  EXPECT_TRUE(success);
  EXPECT_EQ("OK", status);
}

TEST(RayTracerTest, TurbineMeshTest) {
  SceneLoader& loader = SceneLoader::GetInstance();
  std::string path = "../assets/blade.ply";
  std::string status = "";
  Scene scene;
  std::cout << "loading" << std::endl;
  bool success = loader.LoadScene(path, scene, status);
  EXPECT_TRUE(success);
  EXPECT_EQ("OK", status);
  std::cout << "done loading" << std::endl;
  int image_width = 512;
  int image_height = 512;
  glm::vec3 eye_pos = glm::vec3(-274.564f, -282.243f, 950.0f);
  glm::vec3 at_pos = glm::vec3(-274.564f, -282.243f, 254.327f);
  glm::vec3 up_dir = glm::vec3(0.0f, 1.0f, 0.0f);
  std::cout << "set camera" << std::endl;
  glm::mat4x4 look_at = LookAt(eye_pos, at_pos, up_dir);
  Camera camera(image_width, image_height, Orthographic(0.0f, 1.0f), look_at);
  std::cout << "camera set" << std::endl;

  Light point_light;
  glm::vec3 point_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  point_light.ka = point_light_color;
  point_light.kd = point_light_color;
  point_light.ks = point_light_color;
  point_light.ray = Ray(glm::vec3(0.0, -400.0, 400.0f), glm::vec3(0.0f));
  point_light.type = Light::kPoint;
  point_light.attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
      0.000045492f);

  Light directional_light;
  glm::vec3 directional_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  directional_light.ka = directional_light_color;
  directional_light.kd = directional_light_color;
  directional_light.ks = directional_light_color;
  directional_light.ray = Ray(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, -1.0f));
  directional_light.type = Light::kDirectional;

  scene.AddLight(point_light);
  scene.AddLight(directional_light);

  Trimesh* trimesh = static_cast<Trimesh*>(scene.scene_objects()[0]);
  TestOctree octree;
  std::cout << "Building octree" << std::endl;
  octree.Build(trimesh->faces());
  //octree.set_trace(true);
  //octree.Print(std::cout);
  std::cout << "Octree built.\n";
  std::cout << "bounds = " << octree.GetBounds() << " center = "
      << octree.GetBounds().GetCenter() << std::endl;
  trimesh->set_accelerator(&octree);
  Image image;
  image.resize(image_width, image_height);
  RayTracer ray_tracer(&scene, &camera);
  ray_tracer.set_display_progress(!use_timing);
  ray_tracer.set_display_stats(!use_timing);
  ray_tracer.set_background_color(glm::vec3(0.05f, 0.05f, 0.05f));

  timeval t_start, t_finish;
  if (use_timing)
    gettimeofday(&t_start, NULL);
  std::cout << "Rendering..." << std::endl;
  ray_tracer.Render(image);
  if (use_timing) {
    gettimeofday(&t_finish, NULL);
    float sec = t_finish.tv_sec - t_start.tv_sec + t_finish.tv_usec / 1000000.0f
        - t_start.tv_usec / 1000000.0f;
    std::cout << "Render time:" << sec << std::endl;
  } else
    std::cout << "Done." << std::endl;

  ImageStorage& storage = ImageStorage::GetInstance();
  success = storage.WriteImage("blade_octree.jpg", image, status);
  EXPECT_TRUE(success);
  EXPECT_EQ("OK", status);
}
} // namespace ray

