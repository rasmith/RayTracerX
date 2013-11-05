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
    Scene();
    void AddCamera(const Camera& camera);
    void AddMaterial(
            const std::string& name,
            int mesh_id,
            const Material& material);
    void AddMesh(Trimesh* mesh);
    const std::vector<Camera>& cameras() const;
    const std::vector<Light>& lights() const;
    const MaterialList& material_list() const;
    const std::vector<Trimesh*>& meshes() const;
private:
    std::vector<Camera> cameras_;
    std::vector<Light> lights_;
    std::vector<Trimesh*> meshes_;
    std::vector<MaterialList> material_lists_;
};
} // namespace ray
#endif /* SCENE_HPP_ */
