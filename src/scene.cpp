/*
 * scene.cpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */
namespace ray {
Scene::Scene() {
}

void Scene::AddCamera(const Camera& camera) {
    cameras_.push_back(camera);
}

void Scene::AddMaterial(
        const std::string& name,
        int mesh_id,
        const Material& material) {
    MaterialList& list = material_lists_[mesh_id];
    list.AddMaterial(name, material);
}

void Scene::AddMesh(Trimesh* mesh) {
    meshes_.push_back(mesh);
    material_lists_.push_back(MaterialList());
}

const std::vector<Camera>& Scene::cameras() const {
    return cameras_;
}

const std::vector<Light>& Scene::lights() const {
    return lights_;
}

const MaterialList& Scene::material_list() const {
    return material_lists_;
}

const std::vector<Trimesh*>& Scene::meshes() const {
    return meshes_;
}
} // namespace ray

