/*
 * io_utils.cpp
 *
 *  Created on: Nov 6, 2013
 *      Author: agrippa
 */
#include <iostream>
#include <glm/glm.hpp>
#include "ray.hpp"
namespace ray {
std::ostream& operator <<(std::ostream& out, glm::vec3 v) {
    out << "[" << v[0] << "," << v[1] << "," << v[2] << "]";
    return out;
}

std::ostream& operator <<(std::ostream& out, glm::vec4 v) {
    out << "[" << v[0] << "," << v[1] << "," << v[2] << "," << v[3] << "]";
    return out;
}

std::ostream& operator <<(std::ostream& out, glm::mat4x4 m) {
    glm::mat4x4 T = glm::transpose(m);
    out << "[" << T[0] << "," << T[1] << "," << T[2] << "," << T[3] << "]";
    return out;
}

std::ostream& operator <<(std::ostream& out, Ray r) {
    out << "[o = " << r.origin() << ", d = " << r.direction() << "]";
    return out;
}
} // namespace ray
