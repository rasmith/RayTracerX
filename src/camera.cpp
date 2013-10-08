/*
 * camera.cpp
 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
namespace ray {
Camera::Camera() :
        near_(0.001f), far_(1.0f), aspect_(1.5f), fov_(45.0f), at_(0.0f, 0.0f,
                -0.5f), position_(0.0f, 0.0f, 0.0f), up_(0.0f, 1.0f, 0.0f) ,
                view_ (glm::lookAt(position_, at_, up_)),
                transform_(glm::inverse((view_))), project_(glm::mat4x4(1.0f)),
                unproject_(glm::mat4x4(1.0f)) {
}

Camera::Camera(const Camera& cam) :
        near_(cam.near_), far_(cam.far_), aspect_(cam.aspect_), fov_(cam.fov_),
        at_(cam.at_), position_(cam.position_), up_(cam.up_), view_(cam.view_),
        transform_(cam.transform_), project_(cam.project_),
        unproject_(cam.unproject_) {
}

Camera::Camera(float near, float far, float aspect, float fov,
        const glm::vec3& at, const glm::vec3& position, const glm::vec3& up) :
        near_(near), far_(far), aspect_(aspect), fov_(fov), at_(at),
        position_(position), up_(up), view_(glm::lookAt(position_, at_, up_)),
        transform_(glm::inverse((view_))),
        project_(glm::perspective(fov_, aspect_, near_, far_)),
        unproject_(glm::inverse(project_)) {
}

glm::vec3 Camera::Project(const glm::vec3& point) {
}

glm::vec3 Camera::Unproject(const glm::vec3& point) {
}

float Camera::aspect() const {
    return aspect_;
}

void Camera::set_aspect(float aspect) {
    aspect_ = aspect;
}

const glm::vec3& Camera::at() const {
    return at_;
}

void Camera::set_at(const glm::vec3& at) {
    at_ = at;
}

float Camera::far() const {
    return far_;
}

void Camera::set_far(float far) {
    far_ = far;
}

float Camera::fov() const {
    return fov_;
}

void Camera::set_fov(float fov) {
    fov_ = fov;
}

const std::string& Camera::name() const {
    return name_;
}

void Camera::set_name(const std::string& name) {
    name_ = name;
}

float Camera::near() const {
    return near_;
}

void Camera::set_near(float near) {
    near_ = near;
}

const glm::vec3& Camera::position() const {
    return position_;
}

void Camera::set_position(const glm::vec3& position) {
    position_ = position;
}

const glm::mat4x4& Camera::transform() const {
    return transform_;
}

void Camera::set_transform(const glm::mat4x4& transform) {
    transform_ = transform;
}

const glm::vec3& Camera::up() const {
    return up_;
}

void Camera::set_up(const glm::vec3& up) {
    up_ = up;
}

const glm::mat4x4& Camera::project() const {
    return project_;
}

const glm::mat4x4& Camera::unproject() const {
    return unproject_;
}

const glm::mat4x4& Camera::view() const {
    return view_;
}

} // namespace ray

