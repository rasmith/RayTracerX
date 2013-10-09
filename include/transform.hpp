/*
 * transform.hpp
 *
 *  Created on: Oct 8, 2013
 *      Author: agrippa
 */

#ifndef TRANSFORM_HPP_
#define TRANSFORM_HPP_

namespace ray {
static glm::mat4x4 Inverse(const glm::mat4x4& transform);
static glm::mat4x4 Identity();
static glm::mat4x4 Orthographic(float near, float far);
static glm::mat4x4 Perspective(float near, float far, float aspect,
        float fov);
static glm::mat4x4 Rotate(float axis, const glm::vec3& angle);
static glm::mat4x4 RotateX(float angle);
static glm::mat4x4 RotateY(float angle);
static glm::mat4x4 RotateZ(float angle);
static glm::mat4x4 Translate(const glm::vec3& direction);
static glm::mat4x4 Scale(const glm::vec3& scale);
static glm::mat4x4 LookAt(
        const glm::vec3& position,
        const glm::vec3& at,
        const glm::vec3& up);
}

#endif /* TRANSFORM_HPP_ */
