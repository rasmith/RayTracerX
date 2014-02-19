/*
 * scene_loader_test.cpp
 *
 *  Created on: Nov 5, 2013
 *      Author: agrippa
 */

#include <climits>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "io_utils.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "scene_utils.hpp"
using ::testing::AnyOf;
namespace ray {
TEST(SceneLoaderTest, SimpleReadTest) {
    SceneLoader& loader = SceneLoader::GetInstance();
    std::string path = "../assets/cube.ply";
    std::string status = "";
    Scene scene;
    bool success = loader.LoadScene(path, scene, status);
    EXPECT_TRUE(success);
    EXPECT_EQ("OK", status);
    EXPECT_EQ(scene.cameras().size(), 0u);
    EXPECT_EQ(scene.lights().size(), 0u);
    EXPECT_EQ(scene.material_list().materials.size(), 1u);
    const std::vector<SceneShape*>& shapes = scene.scene_objects();
    EXPECT_EQ(1u, shapes.size());
    Trimesh* mesh = static_cast<Trimesh*>(shapes[0]);
    // Assimp can duplicate vertices, so either 8 or 24 is fine
    EXPECT_THAT(mesh->num_vertices(), AnyOf(8, 24));
    // Make sure we have all the vertices
    bool check[8];
    memset(check, 0, sizeof(bool) * 8);
    for (int i = 0; i < mesh->num_vertices(); ++i) {
        const glm::vec3& v = mesh->vertices()[i];
        uint32_t I = static_cast<uint32_t>(v[0]);
        uint32_t J = static_cast<uint32_t>(v[1]);
        uint32_t K = static_cast<uint32_t>(v[2]);
        uint32_t idx = (K << 2) | (J << 1) | I;
        check[idx] = true;
    }
    bool have_all_vertices = true;
    for (int i = 0; i < 8; ++i) {
        have_all_vertices &= check[i];
    }
    EXPECT_TRUE(have_all_vertices);
    EXPECT_EQ(12, mesh->num_faces());
}
TEST(SceneLoaderTest, BlenderReadTest) {
    SceneLoader& loader = SceneLoader::GetInstance();
    std::string path = "../assets/bunny.obj";
    std::string status = "";
    Scene scene;
    bool success = loader.LoadScene(path, scene, status);
    EXPECT_TRUE(success);
    EXPECT_EQ("OK", status);
}
} // namespace ray

