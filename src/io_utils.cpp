/*
 * io_utils.cpp
 *
 *  Created on: Nov 6, 2013
 *      Author: agrippa
 */
#include <iostream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "io_utils.hpp"
#include "ray.hpp"
namespace ray {
std::ostream& operator <<(std::ostream& out, const glm::vec2& v) {
  out << "[" << v[0] << "," << v[1] << "]";
  return out;
}

std::ostream& operator <<(std::ostream& out, const glm::vec3& v) {
  out << "[" << v[0] << "," << v[1] << "," << v[2] << "]";
  return out;
}

std::ostream& operator <<(std::ostream& out, const glm::vec4& v) {
  out << "[" << v[0] << "," << v[1] << "," << v[2] << "," << v[3] << "]";
  return out;
}

std::ostream& operator <<(std::ostream& out, const glm::mat4x4& m) {
  glm::mat4x4 T = glm::transpose(m);
  out << "[" << T[0] << "," << T[1] << "," << T[2] << "," << T[3] << "]";
  return out;
}

std::ostream& operator <<(std::ostream& out, const glm::mat3x3& m) {
  glm::mat3x3 T = glm::transpose(m);
  out << "[" << T[0] << "," << T[1] << "," << T[2] << "," << "]";
  return out;
}
} // namespace ray
