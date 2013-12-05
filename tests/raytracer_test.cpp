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
#include "raytracer.hpp"
#include "scene_utils.hpp"
#include "transform.hpp"
namespace ray {
TEST(RayTracerTest, SphereTest) {
    Scene scene;

    Sphere sphere(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);
    Material sphere_material;
    sphere_material.kd = glm::vec3(0.4f, 0.8f, 0.2f);
    sphere_material.ks = glm::vec3(1.0f, 1.0f, 0.0f);
    sphere_material.ns = 64;

    Ray test_ray1(glm::vec3(0.0f, 0.0f, -4.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    Ray test_ray2(glm::vec3(0.0f, 1.0f, -4.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    Isect isect;
    bool hit1 = sphere.Intersect(test_ray1, isect);
    EXPECT_TRUE(hit1);
    bool hit2 = sphere.Intersect(test_ray2, isect);
    EXPECT_FALSE(hit2);

    glm::vec3 eye_pos = glm::vec3(0.0f, 0.0f, -4.0f);
    glm::vec3 at_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up_dir = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4x4 look_at = LookAt(eye_pos, at_pos, up_dir);
    std::cout << "look_at = " << look_at << std::endl;
    std::cout << "transform = " << glm::inverse(look_at) << std::endl;
    Camera camera(512, 512, Orthographic(0.0f, 1.0f), look_at);
    std::cout << "direction = "
            << glm::inverse(look_at) * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)
            << std::endl;

    Light point_light;
    glm::vec3 point_light_color = glm::vec3(1.0f, 0.3f, 0.3f);
    point_light.ka = point_light_color;
    point_light.kd = point_light_color;
    point_light.ks = point_light_color;
    point_light.ray = Ray(glm::vec3(-2.0f, 2.0f, -2.0f), glm::vec3(0.0f));
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

    scene.AddSceneObject(&sphere);
    scene.AddLight(point_light);
    scene.AddLight(directional_light);
    scene.AddMaterial("sphere_material", sphere_material);

    Image image;
    image.resize(512, 512);
    RayTracer ray_tracer(&scene, &camera);
    ray_tracer.Render(image);

    std::string status;
    ImageStorage& storage = ImageStorage::GetInstance();
    storage.WriteImage("sphere.jpg", image, status);
}
TEST(RayTracerTest, TriangleTest) {
    Scene scene;

    glm::vec3 a = glm::vec3(-0.4f, -0.4f, 0.0f);
    glm::vec3 b = glm::vec3(0.4f, -0.4f, 0.0f);
    glm::vec3 c = glm::vec3(0.0f, 0.4f, 0.0f);

    Triangle triangle(a, b, c);
    Material triangle_material;
    triangle_material.kd = glm::vec3(0.4f, 0.8f, 0.2f);
    triangle_material.ks = glm::vec3(1.0f, 1.0f, 0.0f);
    triangle_material.ns = 64;

    Ray test_ray1(glm::vec3(0.0f, 0.25f, -4.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    Ray test_ray2(glm::vec3(0.0f, 1.0f, -4.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    Isect isect;
    bool hit1 = triangle.Intersect(test_ray1, isect);
    EXPECT_TRUE(hit1);
    bool hit2 = triangle.Intersect(test_ray2, isect);
    EXPECT_FALSE(hit2);

    glm::vec3 eye_pos = glm::vec3(0.0f, 0.0f, -4.0f);
    glm::vec3 at_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up_dir = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4x4 look_at = LookAt(eye_pos, at_pos, up_dir);
    Camera camera(512, 512, Orthographic(0.0f, 1.0f), look_at);

    Light point_light;
    glm::vec3 point_light_color = glm::vec3(1.0f, 0.3f, 0.3f);
    point_light.ka = point_light_color;
    point_light.kd = point_light_color;
    point_light.ks = point_light_color;
    point_light.ray = Ray(glm::vec3(0.25f, -0.25f, -2.0f), glm::vec3(0.0f));
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

    scene.AddSceneObject(&triangle);
    scene.AddLight(point_light);
    scene.AddLight(directional_light);
    scene.AddMaterial("triangle_material", triangle_material);

    Image image;
    image.resize(512, 512);
    RayTracer ray_tracer(&scene, &camera);
    ray_tracer.Render(image);

    std::string status;
    ImageStorage& storage = ImageStorage::GetInstance();
    storage.WriteImage("triangle.jpg", image, status);
}
} // namespace ray

