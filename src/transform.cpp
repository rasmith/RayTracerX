/*
 * transform.cpp
 *
 *  Created on: Oct 8, 2013
 *      Author: agrippa
 */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

glm::mat4x4 LookAt(
        const glm::vec3& position,
        const glm::vec3& at,
        const glm::vec3& up) {
    glm::vec3 Z = glm::normalize(glm::vec3(at - position));
    glm::vec3 X = glm::normalize(glm::cross(Z, up));
    glm::vec3 Y = glm::normalize(glm::cross(X, Z));
    glm::mat3x3 R_T = glm::mat3x3(X, Y, Z);
    glm::vec3 t_view = -R_T * position;
    glm::mat4x4 V = glm::mat4x4(R_T);
    V[3] = glm::vec4(glm::vec3(t_view), 1.0f);
    return V;
}
}

