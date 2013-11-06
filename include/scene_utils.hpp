/*
 * scene_utils.hpp
 *
 *  Created on: Oct 22, 2013
 *      Author: agrippa
 */

#ifndef SCENE_UTILS_HPP_
#define SCENE_UTILS_HPP_
#include <assimp/scene.h>
#include <assimp/camera.h>
#include "scene.hpp"
#include "types.hpp"
namespace ray {
class SceneLoader {
public:
    static SceneLoader& GetInstance();
    ~SceneLoader();
    bool LoadScene(
            const std::string& file_name,
            Scene& scene,
            std::string& status);
private:
    SceneLoader();
    SceneLoader(const SceneLoader&);
    void ImportCamera(Scene& scene, const aiCamera* const camera);
    void ImportLight(Scene& scene, const aiLight* const light);
    void ImportMaterial(Scene& scene, const aiMaterial* const material);
    void ImportMesh(Scene& scene, const aiMesh* const mesh);
};
}
#endif /* SCENE_UTILS_HPP_ */
