/*
 * scene.hpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */
#ifndef SCENE_HPP_
#define SCENE_HPP_
#include <vector>
#include "mesh.hpp"
#include "material.hpp"
#include "camera.hpp"
namespace ray {
class Scene {
public:
private:
    std::vector<Camera> cameras_;
    std::vector<Light> lights_;
    std::vector<Trimesh*> meshes_;
    MaterialList material_list_;
};
} // namespace ray
#endif /* SCENE_HPP_ */
