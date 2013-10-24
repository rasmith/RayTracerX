/*
 * scene_utils.hpp
 *
 *  Created on: Oct 22, 2013
 *      Author: agrippa
 */

#ifndef SCENE_UTILS_HPP_
#define SCENE_UTILS_HPP_
#include "scene.hpp"
#include "types.hpp"
namespace ray {
class SceneLoader {
public:
    static SceneLoader& GetInstance();
    ~SceneLoader();
    bool LoadScene(
            const std::string& file_name,
            const Scene& image,
            std::string& status);
private:
    SceneLoader();
    SceneLoader(const SceneLoader&);
};
}
#endif /* SCENE_UTILS_HPP_ */
