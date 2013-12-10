/*
 * light.cpp

 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */
#include <ostream>
#include <string>
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
std::ostream& operator<<(std::ostream& out, const Light& l) {
    out << "[";
    static std::string type_to_string[Light::kNumLightTypes] = { "kUndefined",
            "kDirectional", "kPoint", "kSpot" };
    out << "name:" << l.name << ",";
    out << "ray:" << l.ray << ",";
    out << "type:" << type_to_string[l.type] << ",";
    out << "ka:" << l.ka << ",";
    out << "kd:" << l.ks << ",";
    out << "ks:" << l.ks << ",";
    out << "spot_coeffs:" << l.spot_coefficients << ",";
    out << "atten_coeffs:" << l.attenuation_coefficients;
    out << "]";
    return out;
}
}
