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

SceneShape::SceneShape() :
    material_(NULL) {
}

SceneShape::SceneShape(Material* const & material) :
    material_(material) {
}

Material* const & SceneShape::material() const {
  return material_;
}

void SceneShape::set_material(Material* const & material) {
  material_ = material;
}

Shape* const & MaterialShape::shape() const {
  return shape_;
}

MaterialShape::MaterialShape() :
    shape_(NULL) {
}

MaterialShape::MaterialShape(Shape* const & shape, Material* const & material) :
    SceneShape(material), shape_(shape) {
}

void MaterialShape::set_shape(Shape* const & shape) {
  shape_ = shape;
}

bool MaterialShape::Intersect(const Ray& ray, Isect& isect) const {
  return shape_->Intersect(ray, isect);
}

void MaterialShape::Print(std::ostream& out) const {
  out << "MatShape: M:" << *material() << " S:" << *shape();
}

Scene::Scene() :
    cameras_(), lights_(), scene_shapes_(), material_list_() {
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

void Scene::AddSceneShape(SceneShape* const & shape) {
  scene_shapes_.push_back(shape);
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

MaterialList& Scene::material_list() {
  return material_list_;
}

const std::vector<SceneShape*>& Scene::scene_objects() const {
  return scene_shapes_;
}

bool Scene::Intersect(const Ray& ray, Isect& isect) {
  bool hit = false;
  Isect current;
  Isect best;
  best.t_hit = std::numeric_limits<float>::max();
  for (uint32_t i = 0; i < scene_shapes_.size(); ++i) {
    if (scene_shapes_[i]->Intersect(ray, current)
        && current.t_hit < best.t_hit) {
      best = current;
      hit = true;
      if (!best.mat)
        best.mat = scene_shapes_[i]->material();
    }
  }
  if (hit) {
    isect = best;
  }
  return hit;
}

std::ostream& operator<<(std::ostream& out, const Scene& scene) {
  out << "[mats:" << scene.material_list() << "\n";
  std::vector<Camera>::const_iterator cam_iter = scene.cameras().begin();
  out << "cams:[";
  for (; cam_iter != scene.cameras().end(); ++cam_iter) {
    if (cam_iter != scene.cameras().begin())
      out << ",\n";
    out << *cam_iter;
  }
  out << "]\n";
  out << "lights:[";
  std::vector<Light>::const_iterator light_iter = scene.lights().begin();
  for (; light_iter != scene.lights().end(); ++light_iter) {
    if (light_iter != scene.lights().begin())
      out << ",\n";
    out << *light_iter;
  }
  out << "]\n";
  out << "scene_objs:[";
  std::vector<SceneShape*>::const_iterator shape_iter = scene.scene_objects()
      .begin();
  for (; shape_iter != scene.scene_objects().end(); ++shape_iter) {
    if (shape_iter != scene.scene_objects().begin())
      out << ",\n";
    out << **shape_iter;
  }
  out << "]\n";
  out << "]";
  return out;
}
} // namespace ray
