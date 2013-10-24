/*
 * scene_utils.cpp
 *
 *  Created on: Oct 23, 2013
 *      Author: agrippa
 */
#include "scene_utils.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
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
        const Scene& scene,
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
    return (NULL != assimp_scene);
}
} // namespace ray

