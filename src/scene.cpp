/*
 * scene.cpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */
#include <string>
#include <vector>
#include "camera.hpp"
#include "light.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "scene.hpp"
namespace ray {
Scene::Scene() :
        cameras_(), lights_(), meshes_(), material_list_() {
}

void Scene::AddCamera(const Camera& camera) {
    cameras_.push_back(camera);
}

void Scene::AddLight(const Light& light) {
    lights_.push_back(light);
}

void Scene::AddMaterial(const std::string& name, const Material& material) {
    material_list_.AddMaterial(name, material);
}

void Scene::AddMesh(Trimesh* mesh) {
    meshes_.push_back(mesh);
}

const std::vector<Camera>& Scene::cameras() const {
    return cameras_;
}

const std::vector<Light>& Scene::lights() const {
    return lights_;
}

const MaterialList& Scene::material_list() const {
    return material_list_;
}

const std::vector<Trimesh*>& Scene::meshes() const {
    return meshes_;
}
} // namespace ray

