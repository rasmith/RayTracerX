/*
 * light.hpp
 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */

#ifndef LIGHT_HPP_
#define LIGHT_HPP_
#include "types.hpp"
namespace ray {
struct Light {
    enum LightType {
        kUndefined, kDirectional, kPoint, kSpot
    };
    glm::vec3 ka;
    glm::vec3 kd;
    glm::vec3 ks;
    Ray ray;
    std::string name;
    LightType type;
};
} // namespace ray
#endif /* LIGHT_HPP_ */