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
#include "mesh.hpp"
#include "material.hpp"
#include "ray.hpp"
#include "shape.hpp"
namespace ray {
class Scene {
public:
    Scene();
    void AddCamera(const Camera& camera);
    void AddLight(const Light& light);
    void AddMaterial(const std::string& name, const Material& material);
    void AddSceneObject(Shape* mesh);
    const std::vector<Camera>& cameras() const;
    const std::vector<Light>& lights() const;
    const MaterialList& material_list() const;
    const std::vector<Shape*>& scene_objects() const;
    bool Intersect(const Ray& ray, Isect& isect);
private:
    std::vector<Camera> cameras_;
    std::vector<Light> lights_;
    std::vector<Shape*> scene_objects_;
    MaterialList material_list_;
};
std::ostream& operator<<(std::ostream& out, const Scene& scene);
} // namespace ray
#endif /* SCENE_HPP_ */
