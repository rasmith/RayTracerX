/*
 * light.cpp

 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */
#include <iostream>
#include "io_utils.hpp"
#include "light.hpp"
#include "types.hpp"
namespace ray {
glm::vec3 Light::Direction(const Light& light, const glm::vec3& hit_point) {
    glm::vec3 dir = glm::vec3(0.0f);
    switch (light.type) {
    case Light::kPoint:
        dir = glm::normalize(light.ray.origin() - hit_point);
        break;
    case Light::kDirectional:
        dir = glm::normalize(-light.ray.direction());
        break;
    default:
        dir = glm::vec3(0.0f);
    }
    return dir;
}
}
