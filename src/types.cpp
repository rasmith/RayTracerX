/*
 * types.cpp
 *
 *  Created on: Apr 1, 2014
 *      Author: agrippa
 */
#include <glm/glm.hpp>
#include <glm/detail/precision.hpp>
#include "types.hpp"

namespace ray {
glm::vec3 operator+(const glm::ivec3& i, const glm::vec3& r) {
  return glm::vec3(i[0] + r[0], i[1] + r[1], i[2] + r[2]);
}

glm::vec3 operator+(const glm::vec3& r, const glm::ivec3& i) {
  return i + r;
}

glm::vec3 operator*(const glm::ivec3& i, const glm::vec3& r) {
  return glm::vec3(i[0] * r[0], i[1] * r[1], i[2] * r[2]);
}

glm::vec3 operator*(const glm::vec3& r, const glm::ivec3& i) {
  return i * r;
}

glm::vec3 operator*(float f, const glm::ivec3& i) {
  return glm::vec3(f * i[0], f * i[1], f * i[2]);
}
}

