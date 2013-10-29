/*
 * scene_utils.cpp
 *
 *  Created on: Oct 23, 2013
 *      Author: agrippa
 */
#include "scene_utils.hpp"
#include "types.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/camera.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#define AI_CONFIG_PP_SBP_REMOVE \
    (aiPrimitiveType_POINTS | aiPrimitiveType_LINES)

namespace ray {
SceneLoader::SceneLoader() {
}

SceneLoader::SceneLoader(const SceneLoader&) {
}

SceneLoader& SceneLoader::GetInstance() {
    static SceneLoader instance;
    static bool is_initialized = false;
    if (!is_initialized) {
        is_initialized = true;
    }
    return instance;
}

SceneLoader::~SceneLoader() {
}

bool SceneLoader::LoadScene(
        const std::string& file_name,
        Scene& scene,
        int image_width,
        int image_height,
        std::string& status) {
    struct aiLogStream stream;
    Assimp::Importer importer;
    const aiScene* assimp_scene = importer.ReadFile(file_name,
            aiProcess_Triangulate | aiProcess_GenNormals
                    | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType
                    | aiProcess_ValidateDataStructure
                    | aiProcess_ImproveCacheLocality
                    | aiProcess_RemoveRedundantMaterials
                    | aiProcess_FixInfacingNormals | aiProcess_FindDegenerates
                    | aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes);
    status = std::string(importer.GetErrorString());
    if (!status.empty() || NULL == assimp_scene) {
        return false;
    }
    for (int i = 0; i < assimp_scene->mNumCameras; ++i) {
        ImportCamera(scene, assimp_scene->mCameras[i]);
    }
    for (int i = 0; i < assimp_scene->mNumLights; ++i) {
        ImportLight(scene, assimp_scene->mLights[i]);
    }
    for (int i = 0; i < assimp_scene->mNumMaterials; ++i) {
        ImportMaterial(scene, assimp_scene->mMaterials[i]);
    }
    for (int i = 0; i < assimp_scene->mNumMeshes; ++i) {
        ImportMesh(scene, assimp_scene->mMeshes[i]);
    }
    return true;
}

void SceneLoader::ImportCamera(Scene& scene, const aiCamera* const camera) {
    float width = camera->mAspect;
    float height = 1.0;
    glm::mat4x4 persp = glm::perspective(camera->mHorizontalFOV,
            camera->mAspect, camera->mClipPlaneNear, camera->mClipPlaneFar);
    aiVector3D ai_at = camera->mLookAt;
    aiVector3D ai_pos = camera->mPosition;
    aiVector3D ai_up = camera->mUp;
    glm::vec3 at = glm::vec3(ai_at[0], ai_at[1], ai_at[2]);
    glm::vec3 pos = glm::vec3(ai_pos[0], ai_pos[1], ai_pos[3]);
    glm::vec3 up = glm::vec3(ai_up[0], ai_up[1], ai_up[2]);
    glm::mat4x4 look_at = glm::lookAt(pos, at, up);
    scene.AddCamera(Camera(width, height, persp, look_at));
}

void SceneLoader::ImportLight(Scene& scene, const aiLight* const light) {
    Light L;
    aiColor3D ai_color_ka = light->mColorAmbient;
    aiColor3D ai_color_kd = light->mColorDiffuse;
    aiColor3D ai_color_ks = light->mColorSpecular;
    aiVector3D ai_direction = light->mDirection;
    aiVector3D ai_position = light->mPosition;
    aiString ai_name = light->mName;
    aiLightSourceType ai_light_type = light->mType;
    L.ka = glm::vec3(ai_color_ka[0], ai_color_ka[1], ai_color_ka[2]);
    L.ks = glm::vec3(ai_color_ks[0], ai_color_ks[1], ai_color_ks[2]);
    L.kd = glm::vec3(ai_color_kd[0], ai_color_kd[1], ai_color_kd[2]);
    L.name = std::string(ai_name.C_Str());
    L.type = static_cast<Light::LightType>(ai_light_type);
    L.ray = Ray(glm::vec3(ai_position[0], ai_position[1], ai_position[2]),
            glm::vec3(ai_direction[0], ai_direction[1], ai_direction[2]));
    L.attenuation_coefficients = glm::vec3(light->mAttenuationConstant,
            light->mAttenuationLinear, light->mAttenuationQuadratic);
    L.spot_coefficients = glm::vec3(light->mAngleInnerCone,
            light->mAngleOuterCone);
}

void SceneLoader::ImportMaterial(
        Scene& scene,
        const aiMaterial* const material) {
}

void SceneLoader::ImportMesh(Scene& scene, const aiMesh* const mesh) {
}

} // namespace ray

