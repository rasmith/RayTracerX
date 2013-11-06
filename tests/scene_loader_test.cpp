/*
 * scene_loader_test.cpp
 *
 *  Created on: Nov 5, 2013
 *      Author: agrippa
 */

#include <climits>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "gtest/gtest.h"
#include "scene_utils.hpp"

namespace ray {
TEST(SceneLoaderTest, SceneReadTest) {
    SceneLoader& loader = SceneLoader::GetInstance();
    std::string path = "../assets/cube.ply";
    std::string status = "";
    Scene scene;
    bool success = loader.LoadScene(path, scene, status);
    EXPECT_TRUE(success);
    EXPECT_EQ("OK", status);
    EXPECT_EQ(scene.cameras().size(), 0u);
    EXPECT_EQ(scene.lights().size(), 0u);
    EXPECT_EQ(scene.material_list().materials.size(), 0u);
    const std::vector<Trimesh*>& meshes = scene.meshes();
    EXPECT_EQ(meshes.size(), 1u);
    Trimesh* mesh = meshes[0];
    EXPECT_EQ(mesh->num_vertices(), 8);
    EXPECT_EQ(mesh->num_faces(), 12);
}
} // namespace ray

