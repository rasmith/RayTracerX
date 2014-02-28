/*
 * transform.hpp
 *
 *  Created on: Oct 8, 2013
 *      Author: agrippa
 */
#ifndef TRANSFORM_HPP_
#define TRANSFORM_HPP_
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace ray {
glm::mat4x4 Inverse(const glm::mat4x4& transform);
glm::mat4x4 Identity();
glm::mat4x4 Orthographic(float near, float far);
glm::mat4x4 Perspective(float near, float far, float aspect, float fov);
glm::mat4x4 Rotate(float axis, const glm::vec3& angle);
glm::mat4x4 RotateX(float angle);
glm::mat4x4 RotateY(float angle);
glm::mat4x4 RotateZ(float angle);
glm::mat4x4 Translate(const glm::vec3& direction);
glm::mat4x4 Scale(const glm::vec3& scale);
glm::mat4x4 LookAt(
        const glm::vec3& position,
        const glm::vec3& at,
        const glm::vec3& up);
}
#endif /* TRANSFORM_HPP_ */
