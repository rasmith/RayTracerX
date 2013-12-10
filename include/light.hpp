/*
 * light.hpp
 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */

#ifndef LIGHT_HPP_
#define LIGHT_HPP_
#include <ostream>
#include <string>

#include <glm/glm.hpp>

#include "ray.hpp"
#include "types.hpp"
namespace ray {
struct Light {
    enum LightType {
        kUndefined, kDirectional, kPoint, kSpot, kNumLightTypes
    };
    // 0 -> inner angle, 1 -> outer angle
    glm::vec3 spot_coefficients;
    // 0 -> constant, 1 -> linear, 2 -> quadratic
    glm::vec3 attenuation_coefficients;
    glm::vec3 ka;
    glm::vec3 kd;
    glm::vec3 ks;
    Ray ray;
    std::string name;
    LightType type;
    static glm::vec3 Direction(const Light& light, const glm::vec3& hit_point);
};
std::ostream& operator<<(std::ostream& out, const Light& l);
} // namespace ray
#endif /* LIGHT_HPP_ */
