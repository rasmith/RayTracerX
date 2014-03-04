/*
 * scene_utils.cpp
 *
 *  Created on: Oct 23, 2013
 *      Author: agrippa
 */
#include "mesh.hpp"
#include "shape.hpp"
#include "scene_utils.hpp"
#include "types.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/camera.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

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

bool SceneLoader::LoadScene(const std::string& file_name, Scene& scene,
    std::string& status) {
  Assimp::Importer importer;
  importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,
      aiPrimitiveType_POINT | aiPrimitiveType_LINE);
  const aiScene* assimp_scene = importer.ReadFile(file_name,
      aiProcess_Triangulate | aiProcess_JoinIdenticalVertices
          | aiProcess_FixInfacingNormals | aiProcess_FindDegenerates
          | aiProcess_ValidateDataStructure);
  // | aiProcess_ImproveCacheLocality
  //  | aiProcess_RemoveRedundantMaterials
  //  | aiProcess_FixInfacingNormals | aiProcess_FindDegenerates
  // | aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes);
  status = std::string(importer.GetErrorString());
  if (!status.empty() || NULL == assimp_scene) {
    return false;
  }
  status = "OK";
  for (uint32_t i = 0; i < assimp_scene->mNumCameras; ++i) {
    ImportCamera(scene, assimp_scene->mCameras[i]);
  }
  for (uint32_t i = 0; i < assimp_scene->mNumLights; ++i) {
    ImportLight(scene, assimp_scene->mLights[i]);
  }
  std::cout << "mNumMaterials = " << assimp_scene->mNumMaterials << std::endl;
  for (uint32_t i = 0; i < assimp_scene->mNumMaterials; ++i) {
    ImportMaterial(scene, assimp_scene->mMaterials[i]);
  }
  std::cout << "mNumMeshes = " << assimp_scene->mNumMeshes << std::endl;
  for (uint32_t i = 0; i < assimp_scene->mNumMeshes; ++i) {
    ImportMesh(scene, assimp_scene->mMeshes[i]);
  }
  return true;
}

void SceneLoader::ImportCamera(Scene& scene, const aiCamera* const camera) {
  float width = camera->mAspect;
  float height = 1.0;
  glm::mat4x4 persp = glm::perspective(camera->mHorizontalFOV, camera->mAspect,
      camera->mClipPlaneNear, camera->mClipPlaneFar);
  aiVector3D ai_at = camera->mLookAt;
  aiVector3D ai_pos = camera->mPosition;
  aiVector3D ai_up = camera->mUp;
  glm::vec3 at = glm::vec3(ai_at[0], ai_at[1], ai_at[2]);
  glm::vec3 pos = glm::vec3(ai_pos[0], ai_pos[1], ai_pos[2]);
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
      light->mAngleOuterCone, 0.0f);
  scene.AddLight(L);
}

void SceneLoader::ImportMaterial(Scene& scene,
    const aiMaterial* const material) {
  aiString ai_name;
  material->Get(AI_MATKEY_NAME, ai_name);
  aiColor4D diffuse_color(0.0f, 0.0f, 0.0f, 1.0f);
  material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color);
  aiColor4D specular_color(0.0f, 0.0f, 0.0f, 1.0f);
  material->Get(AI_MATKEY_COLOR_SPECULAR, specular_color);
  aiColor4D ambient_color(0.0f, 0.0f, 0.0f, 1.0f);
  material->Get(AI_MATKEY_COLOR_AMBIENT, ambient_color);
  aiColor4D emissive_color(0.0f, 0.0f, 0.0f, 1.0f);
  material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive_color);
  float opacity = 0.0f;
  material->Get(AI_MATKEY_OPACITY, opacity);
  float reflectivity = 0.0f;
  material->Get(AI_MATKEY_REFLECTIVITY, reflectivity);
  float refractivity = 0.0f;
  material->Get(AI_MATKEY_REFRACTI, refractivity);
  float shininess = 0.0f;
  material->Get(AI_MATKEY_SHININESS, shininess);
  std::string name;
  name = std::string(ai_name.C_Str());
  Material mat;
  mat.kd = glm::vec3(diffuse_color[0], diffuse_color[1], diffuse_color[2]);
  mat.ks = glm::vec3(specular_color[0], specular_color[1], specular_color[2]);
  mat.ka = glm::vec3(ambient_color[0], ambient_color[1], ambient_color[2]);
  mat.ke = glm::vec3(emissive_color[0], emissive_color[1], emissive_color[2]);
  mat.kr = reflectivity;
  mat.kt = refractivity;
  mat.tr = opacity;
  mat.ns = shininess;
  scene.AddMaterial(name, mat);
}
void SceneLoader::ImportMesh(Scene& scene, const aiMesh* const mesh) {
  Trimesh* trimesh = new Trimesh();
  for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
    aiVector3D v = mesh->mVertices[i];
    trimesh->AddVertex(glm::vec3(v[0], v[1], v[2]));
  }
  std::cout << "mNumVertices = " << mesh->mNumVertices << std::endl;
  if (NULL != mesh->mNormals) {
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
      aiVector3D n = mesh->mNormals[i];
      trimesh->AddNormal(glm::vec3(n[0], n[1], n[2]));
    }
  }
  std::cout << "mNumFaces = " << mesh->mNumFaces << std::endl;
  for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
    aiFace f = mesh->mFaces[i];
    if (3 == f.mNumIndices) {
      trimesh->AddFace(f.mIndices[0], f.mIndices[1], f.mIndices[2]);
    }
  }
  std::cout << "materials.size() = " << scene.material_list().materials.size()
      << std::endl;
  Material* mat = &scene.material_list().materials[mesh->mMaterialIndex];
  trimesh->set_material(mat);
  if (NULL == mesh->mNormals) {
    trimesh->GenNormals();
  }
  scene.AddSceneShape(static_cast<SceneShape*>(trimesh));
}

} // namespace ray

