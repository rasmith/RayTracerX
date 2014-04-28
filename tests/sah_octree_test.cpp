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

typedef SAHOctree<TrimeshFace> TestOctree;

bool use_timing = true;
bool display_progress = false;
bool display_stats = false;
bool use_accelerator = true;
bool print_tree = false;
int num_timings = 1;
int image_width = 1024;
int image_height = 1024;
glm::vec3 background_color(0.5f, 0.0f, 0.5f);
TestOctree::EvaluationPolicy policy = TestOctree::kBinnedSAH;

void ComputeLookAt(Scene& scene, glm::mat4x4& look_at) {
  Trimesh* trimesh = static_cast<Trimesh*>(scene.scene_objects()[0]);
  BoundingBox bounds = trimesh->GetBounds();
  glm::vec3 dir = glm::vec3(-1.0f, 0.0f, 0.0f);
  glm::vec3 at = bounds.GetCenter();
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 eye = at + 0.5f * (bounds.max()[0] - bounds.min()[0]) * dir;
  look_at = LookAt(eye, at, up);
}

void LoadScene(const std::string& path, Scene& scene, Camera& camera,
    Light* lights, int num_lights, glm::vec3& eye, glm::vec3& at, glm::vec3& up,
    bool auto_camera) {
  SceneLoader& loader = SceneLoader::GetInstance();
  std::string status = "";
  std::cout << "loading" << std::endl;
  bool success = loader.LoadScene(path, scene, status);
  if (!success || status != "OK")
    std::cout << "Problem loading scene:" << path << " status:" << status
        << std::endl;
  EXPECT_TRUE(success);
  EXPECT_EQ("OK", status);
  std::cout << "done loading" << std::endl;
  std::cout << "set camera" << std::endl;
  glm::mat4x4 look_at;
  if (auto_camera)
    ComputeLookAt(scene, look_at);
  else
    look_at = LookAt(eye, at, up);
  camera = Camera(image_width, image_height, Orthographic(0.0f, 1.0f), look_at);
  std::cout << "camera set" << std::endl;
  for (int i = 0; i < num_lights; ++i)
    scene.AddLight(lights[i]);
}

void SetupRayTracer(RayTracer& ray_tracer, Scene& scene, Camera& camera,
    TestOctree& octree) {
  if (use_accelerator) {
    Trimesh* trimesh = static_cast<Trimesh*>(scene.scene_objects()[0]);
    std::cout << "Building octree" << std::endl;

    octree.set_evaluation_policy(policy);
    octree.Build(trimesh->faces());
    if (print_tree)
      octree.Print(std::cout);
    std::cout << "Octree built.\n";
    std::cout << "bounds = " << octree.GetBounds() << " center = "
        << octree.GetBounds().GetCenter() << std::endl;
    trimesh->set_accelerator(&octree);
  }
  ray_tracer.set_scene(&scene);
  ray_tracer.set_camera(&camera);
  ray_tracer.set_display_progress(display_progress);
  ray_tracer.set_display_stats(display_stats);
  ray_tracer.set_background_color(background_color);
}

void RunRayTracer(RayTracer& ray_tracer, Image& image) {
  image.Resize(image_width, image_height);
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
}

void SetupAndRun(const std::string& path, const std::string& output,
    Light* lights, int num_lights, glm::vec3& eye, glm::vec3& at, glm::vec3& up,
    bool auto_camera) {
  Scene scene;
  Camera camera;

  LoadScene(path, scene, camera, lights, num_lights, eye, at, up, auto_camera);

  RayTracer ray_tracer;
  TestOctree octree;
  SetupRayTracer(ray_tracer, scene, camera, octree);
  Image image;
  RunRayTracer(ray_tracer, image);

  ImageStorage& storage = ImageStorage::GetInstance();
  std::string status = "";
  bool success = storage.WriteImage(output, image, status);
  if (!success || status != "OK")
    std::cout << "Problem writing image:" << output << " status:" << status
        << std::endl;
  EXPECT_TRUE(success);
  EXPECT_EQ("OK", status);
}

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
  std::string path = "../assets/sphere.obj";
  std::string output = "sphere_octree.jpg";

  glm::vec3 eye = glm::vec3(0.0f, 0.0f, 1.2f);
  glm::vec3 at = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

  int num_lights = 2;
  Light lights[2];

  glm::vec3 point_light_color = glm::vec3(1.0f, 0.3f, 0.3f);
  lights[0].ka = point_light_color;
  lights[0].kd = point_light_color;
  lights[0].ks = point_light_color;
  lights[0].ray = Ray(glm::vec3(0.0, 1.0, 2.0f), glm::vec3(0.0f));
  lights[0].type = Light::kPoint;
  lights[0].attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
      0.000045492f);

  glm::vec3 directional_light_color = glm::vec3(0.2f, 0.2f, 0.2f);
  lights[1].ka = directional_light_color;
  lights[1].kd = directional_light_color;
  lights[1].ks = directional_light_color;
  lights[1].ray = Ray(glm::vec3(0.0f), glm::vec3(0.05f, 0.05f, 0.05f));
  lights[1].type = Light::kDirectional;

  SetupAndRun(path, output, &lights[0], num_lights, eye, at, up, false);
}

TEST(RayTracerTest, BunnyMeshTest) {
  std::string path = "../assets/bunny.ply";
  std::string output = "bunny_octree.jpg";

  glm::vec3 eye = glm::vec3(-0.0168008f, 0.110153f, 0.225f);
  glm::vec3 at = glm::vec3(-0.0168008f, 0.110153f, -0.00148227f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

  int num_lights = 2;
  Light lights[2];

  glm::vec3 point_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[0].ka = point_light_color;
  lights[0].kd = point_light_color;
  lights[0].ks = point_light_color;
  lights[0].ray = Ray(glm::vec3(0.0, 1.0, 2.0f), glm::vec3(0.0f));
  lights[0].type = Light::kPoint;
  lights[0].attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
      0.000045492f);

  glm::vec3 directional_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[1].ka = directional_light_color;
  lights[1].kd = directional_light_color;
  lights[1].ks = directional_light_color;
  lights[1].ray = Ray(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  lights[1].type = Light::kDirectional;

  SetupAndRun(path, output, &lights[0], num_lights, eye, at, up, false);
}
/**
TEST(RayTracerTest, DragonMeshTest) {
  std::string path = "../assets/dragon.ply";
  std::string output = "dragon_octree.jpg";

  glm::vec3 eye = glm::vec3(-0.0058789f, 0.124951f, 0.275f);
  glm::vec3 at = glm::vec3(-0.0058789f, 0.124951f, -0.0046034f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

  int num_lights = 2;
  Light lights[2];

  glm::vec3 point_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[0].ka = point_light_color;
  lights[0].kd = point_light_color;
  lights[0].ks = point_light_color;
  lights[0].ray = Ray(glm::vec3(0.0, -1.0, 2.0f), glm::vec3(0.0f));
  lights[0].type = Light::kPoint;
  lights[0].attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
      0.000045492f);

  glm::vec3 directional_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[1].ka = directional_light_color;
  lights[1].kd = directional_light_color;
  lights[1].ks = directional_light_color;
  lights[1].ray = Ray(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  lights[1].type = Light::kDirectional;

  SetupAndRun(path, output, &lights[0], num_lights, eye, at, up, false);
}

TEST(RayTracerTest, BuddhaMeshTest) {
  std::string path = "../assets/happy.ply";
  std::string output = "buddha_octree.jpg";

  glm::vec3 eye = glm::vec3(-0.0054393f, 0.14769f, 0.275f);
  glm::vec3 at = glm::vec3(-0.0054393f, 0.148769, -0.00669f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

  int num_lights = 2;
  Light lights[2];

  glm::vec3 point_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[0].ka = point_light_color;
  lights[0].kd = point_light_color;
  lights[0].ks = point_light_color;
  lights[0].ray = Ray(glm::vec3(0.0, -2.0, 2.0f), glm::vec3(0.0f));
  lights[0].type = Light::kPoint;
  lights[0].attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
      0.000045492f);

  glm::vec3 directional_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[1].ka = directional_light_color;
  lights[1].kd = directional_light_color;
  lights[1].ks = directional_light_color;
  lights[1].ray = Ray(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 1.0f));
  lights[1].type = Light::kDirectional;

  SetupAndRun(path, output, &lights[0], num_lights, eye, at, up, false);
}

TEST(RayTracerTest, TurbineMeshTest) {
  std::string path = "../assets/blade.ply";
  std::string output = "blade_octree.jpg";

  glm::vec3 eye = glm::vec3(-274.564f, -282.243f, 950.0f);
  glm::vec3 at = glm::vec3(-274.564f, -282.243f, 254.327f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

  int num_lights = 2;
  Light lights[2];

  glm::vec3 point_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[0].ka = point_light_color;
  lights[0].kd = point_light_color;
  lights[0].ks = point_light_color;
  lights[0].ray = Ray(glm::vec3(0.0, -400.0, 400.0f), glm::vec3(0.0f));
  lights[0].type = Light::kPoint;
  lights[0].attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
      0.000045492f);

  glm::vec3 directional_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[1].ka = directional_light_color;
  lights[1].kd = directional_light_color;
  lights[1].ks = directional_light_color;
  lights[1].ray = Ray(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, -1.0f));
  lights[1].type = Light::kDirectional;

  SetupAndRun(path, output, &lights[0], num_lights, eye, at, up, false);
}

TEST(RayTracerTest, SponzaMeshTest) {
  std::string path = "../assets/sponza.obj";
  std::string output = "sponza.jpg";

  glm::vec3 eye, at, up;
  bool auto_camera = false;
  eye = glm::vec3(-13.7738f, 1.08912f, 1.7613f);
  at = glm::vec3(-0.935384f, 1.35026f, -0.606493f);
  up = glm::vec3(-0.019667f, 0.9998f, 0.00362718f);
  int num_lights = 2;
  Light lights[2];

  glm::vec3 point_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[0].ka = point_light_color;
  lights[0].kd = point_light_color;
  lights[0].ks = point_light_color;
  lights[0].ray = Ray(eye, glm::vec3(0.0f));
  lights[0].type = Light::kPoint;
  lights[0].attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
      0.000045492f);

  glm::vec3 directional_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[1].ka = directional_light_color;
  lights[1].kd = directional_light_color;
  lights[1].ks = directional_light_color;
  lights[1].ray = Ray(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, -1.0f));
  lights[1].type = Light::kDirectional;

  SetupAndRun(path, output, &lights[0], num_lights, eye, at, up, auto_camera);
}

TEST(RayTracerTest, CathedralMeshTest) {
  std::string path = "../assets/cathedral.obj";
  std::string output = "cathedral.jpg";

  glm::vec3 eye, at, up;
  eye = glm::vec3(-10.7098f, -13.9444f, 0.299326f);
  at = glm::vec3(-0.192988f, -12.8887f, -0.00787773f);
  up = glm::vec3(-0.0997908f, 0.995004f, 0.00291497f);
  bool auto_camera = false;

  int num_lights = 2;
  Light lights[2];

  glm::vec3 point_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[0].ka = point_light_color;
  lights[0].kd = point_light_color;
  lights[0].ks = point_light_color;
  lights[0].ray = Ray(eye, glm::vec3(0.0f));
  lights[0].type = Light::kPoint;
  lights[0].attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
      0.000045492f);

  glm::vec3 directional_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[1].ka = directional_light_color;
  lights[1].kd = directional_light_color;
  lights[1].ks = directional_light_color;
  lights[1].ray = Ray(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, -1.0f));
  lights[1].type = Light::kDirectional;

  SetupAndRun(path, output, &lights[0], num_lights, eye, at, up, auto_camera);
}
**/
/**
 TEST(RayTracerTest, SanMiguelMeshTest) {
 std::string path = "../assets/san_miguel.obj";
 std::string output = "san_miguel.jpg";

 glm::vec3 eye, at, up;
 eye = glm::vec3(22.4022f, 1.58558f, 13.3874f);
 at = glm::vec3(15.0403f, 1.58558f, 8.88182f);
 up = glm::vec3(0.0f, 1.0f, 0.0f);
 bool auto_camera = false;

 int num_lights = 2;
 Light lights[2];

 glm::vec3 point_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
 lights[0].ka = point_light_color;
 lights[0].kd = point_light_color;
 lights[0].ks = point_light_color;
 lights[0].ray = Ray(eye, glm::vec3(0.0f));
 lights[0].type = Light::kPoint;
 lights[0].attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
 0.000045492f);

 glm::vec3 directional_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
 lights[1].ka = directional_light_color;
 lights[1].kd = directional_light_color;
 lights[1].ks = directional_light_color;
 lights[1].ray = Ray(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, -1.0f));
 lights[1].type = Light::kDirectional;

 SetupAndRun(path, output, &lights[0], num_lights, eye, at, up, auto_camera);
 }

TEST(RayTracerTest, FairyForestMeshTest) {
  std::string path = "../assets/fairy_forest.obj";
  std::string output = "fairy_forest.jpg";

  bool auto_camera = false;
  glm::vec3 eye, at, up;
  eye = glm::vec3(-0.00549206f, 0.626197f, 0.278735f);
  at = glm::vec3(0.0149054f, 0.714661f, 0.151047f);
  up = glm::vec3(-0.0890694f, 0.825336f, 0.557573f);

  int num_lights = 2;
  Light lights[2];

  glm::vec3 point_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[0].ka = point_light_color;
  lights[0].kd = point_light_color;
  lights[0].ks = point_light_color;
  lights[0].ray = Ray(eye, glm::vec3(0.0f));
  lights[0].type = Light::kPoint;
  lights[0].attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
      0.000045492f);

  glm::vec3 directional_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[1].ka = directional_light_color;
  lights[1].kd = directional_light_color;
  lights[1].ks = directional_light_color;
  lights[1].ray = Ray(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, -1.0f));
  lights[1].type = Light::kDirectional;

  SetupAndRun(path, output, &lights[0], num_lights, eye, at, up, auto_camera);
}

TEST(RayTracerTest, ConferenceMeshTest) {
  std::string path = "../assets/conference.obj";
  std::string output = "conference.jpg";

  bool auto_camera = false;
  glm::vec3 eye, at, up;
  eye = glm::vec3(13.6994f,0.196731f,7.99243f);
  at = glm::vec3(13.6938f,11.23f,4.44847f);
  up = glm::vec3(0.00151883f,0.305818f,0.952089f);

  int num_lights = 2;
  Light lights[2];

  glm::vec3 point_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[0].ka = point_light_color;
  lights[0].kd = point_light_color;
  lights[0].ks = point_light_color;
  lights[0].ray = Ray(eye, glm::vec3(0.0f));
  lights[0].type = Light::kPoint;
  lights[0].attenuation_coefficients = glm::vec3(0.25f, 0.003372407f,
      0.000045492f);

  glm::vec3 directional_light_color = glm::vec3(0.4f, 0.4f, 0.4f);
  lights[1].ka = directional_light_color;
  lights[1].kd = directional_light_color;
  lights[1].ks = directional_light_color;
  lights[1].ray = Ray(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, -1.0f));
  lights[1].type = Light::kDirectional;

  SetupAndRun(path, output, &lights[0], num_lights, eye, at, up, auto_camera);
} **/
} // namespace ray

