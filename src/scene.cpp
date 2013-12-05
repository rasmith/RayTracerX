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
        cameras_(), lights_(), scene_objects_(), material_list_() {
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

void Scene::AddSceneObject(Shape* shape) {
    scene_objects_.push_back(shape);
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

const std::vector<Shape*>& Scene::scene_objects() const {
    return scene_objects_;
}

bool Scene::Intersect(const Ray& ray, Isect& isect) {
    bool hit = false;
    Isect current;
    Isect best;
    best.t_hit = std::numeric_limits<float>::max();
    for (uint32_t i = 0; i < scene_objects_.size(); ++i) {
        if (scene_objects_[i]->Intersect(ray, current)
                && current.t_hit < best.t_hit) {
            best = current;
            hit = true;
            best.mat = &material_list_.materials[i];
        }
    }
    if (hit) {
        isect = best;
    }
    return hit;
}
} // namespace ray
