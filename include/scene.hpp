/*
 * scene.hpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */
#ifndef SCENE_HPP_
#define SCENE_HPP_
#include <vector>
#include "camera.hpp"
#include "light.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "shape.hpp"
namespace ray {
class SceneShape: public Shape {
public:
  Material* const & material() const;
  void set_material(Material* const & material);
  bool trace() const;
  void set_trace(bool trace);
protected:
  SceneShape();
  SceneShape(Material* const & material);
  Material* material_;
  bool trace_;
};
class MaterialShape: public SceneShape {
public:
  MaterialShape();
  MaterialShape(Shape* const & shape, Material* const & material);
  Shape* const & shape() const;
  void set_shape(Shape* const & shape);
  virtual bool Intersect(const Ray& ray, Isect& isect) const;
  virtual void Print(std::ostream& out) const;
private:
  Shape* shape_;
};
class Scene {
public:
  Scene();
  void AddCamera(const Camera& camera);
  void AddLight(const Light& light);
  void AddMaterial(const std::string& name, const Material& material);
  void AddSceneShape(SceneShape* const & shape);
  const std::vector<Camera>& cameras() const;
  const std::vector<Light>& lights() const;
  const MaterialList& material_list() const;
  MaterialList& material_list();
  const std::vector<SceneShape*>& scene_objects() const;
  bool Intersect(const Ray& ray, Isect& isect);
  bool trace() const;
  void set_trace(bool trace);
private:
  std::vector<Camera> cameras_;
  std::vector<Light> lights_;
  std::vector<SceneShape*> scene_shapes_;
  MaterialList material_list_;
  bool trace_;
};
std::ostream& operator<<(std::ostream& out, const Scene& scene);
} // namespace ray
#endif /* SCENE_HPP_ */
