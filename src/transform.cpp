/*
 * transform.cpp
 *
 *  Created on: Oct 8, 2013
 *      Author: agrippa
 */
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "io_utils.hpp"
#include "transform.hpp"
namespace ray {
glm::mat4x4 Inverse(const glm::mat4x4& transform) {
  return glm::inverse(transform);
}

glm::mat4x4 Identity() {
  return glm::mat4x4(1.0f);
}

glm::mat4x4 Orthographic(float near, float far) {
  return glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, near, far);
}

glm::mat4x4 Perspective(float fovy, float aspect, float near, float far) {
  return glm::perspective(fovy, aspect, near, far);
}

glm::mat4x4 Rotate(float angle, const glm::vec3& axis) {
  return glm::rotate(Identity(), angle, axis);
}

glm::mat4x4 RotateX(float angle) {
  return Rotate(angle, glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::mat4x4 RotateY(float angle) {
  return Rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4x4 RotateZ(float angle) {
  return Rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f));
}

glm::mat4x4 Translate(const glm::vec3& direction) {
  return glm::translate(Identity(), direction);
}

glm::mat4x4 Scale(const glm::vec3& scale) {
  return glm::translate(Identity(), scale);
}

glm::mat4x4 LookAt(const glm::vec3& position, const glm::vec3& at,
    const glm::vec3& up) {
  std::cout << "LookAt position = " << position << " at = " << at << " up = "
      << up << std::endl;
  glm::vec3 Z = glm::normalize(glm::vec3(position - at));
  std::cout << "Z = " << Z << std::endl;
  glm::vec3 X = glm::normalize(glm::cross(up, Z));
  std::cout << "X = " << X << std::endl;
  glm::vec3 Y = glm::normalize(glm::cross(Z, X));
  std::cout << "Y = " << Y << std::endl;
  glm::mat3x3 R_T = glm::transpose(glm::mat3x3(X, Y, Z));
  std::cout << "R_T = " << R_T << std::endl;
  glm::vec3 t_view = -R_T * position;
  std::cout << "t_view = " << t_view << std::endl;
  glm::mat4x4 V = glm::mat4x4(R_T);
  std::cout << "V = " << V << std::endl;
  V[3] = glm::vec4(glm::vec3(t_view), 1.0f);
  return V;
}
} // namespace ray

