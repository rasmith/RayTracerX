/*
 * io_utils.cpp
 *
 *  Created on: Nov 6, 2013
 *      Author: agrippa
 */
#include <iostream>
#include <glm/glm.hpp>
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
    out << "[" << m[0] << "," << m[1] << "," << m[2] << "," << m[3] << "]";
    return out;
}
} // namespace ray
