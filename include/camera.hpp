/*
 * camera.hpp
 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */

#ifndef CAMERA_HPP_
#define CAMERA_HPP_
#include "types.hpp"
namespace ray {
struct Camera {
    float aspect;
    float near;
    float far;
    float fov;
    std::string name;
    glm::vec3 at;
    glm::vec3 position;
    glm::vec3 up;
    glm::mat4x4 transform;
};
} // namespace ray
#endif /* CAMERA_HPP_ */
