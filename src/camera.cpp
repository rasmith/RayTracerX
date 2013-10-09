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
Camera::Camera() : screen_width_(640.0f), screen_height_(480.0f),
        viewport_(glm::vec4(0.0f, 0.0f, screen_width_, screen_height_)),
        projection_(Identity()), unprojection_(Identity()),
        view_(LookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 1.0f, 0.0f))),
        transform_(Inverse(view_)),
        projection_(Orthographic(0.0f, -1.0f)),
        unprojection_(Inverse(projection_)) {
}

Camera::Camera(const Camera& cam) :  screen_width_(cam.screen_width_),
        screen_height_(cam.screen_height_), viewport_(cam.viewport_),
        projection_(cam.projection_), unprojection_(cam.unprojection_),
        view_(cam.view_), transform_(cam.transform_),
        projection_(cam.projection_), unprojection_(cam.unprojection_)  {
}

Camera::Camera(float screen_width, float screen_height,
        const glm::mat4x4& projection, const glm::mat4x4& look_at) :
            screen_width_(screen_width), screen_height_(screen_height),
            viewport_(glm::vec4(0.0f, 0.0f, screen_width_, screen_height_)),
            view_(look_at), transform_(Inverse(view_)),
            projection_(projection), unprojection_(Inverse(projection_)) {
}

glm::vec3 Camera::Project(const glm::vec3& point) {
    return glm::project(point, view_, projection_, viewport_);
}

glm::vec3 Camera::UnProject(const glm::vec3& point) {
    return glm::unProject(point, view_, projection_, viewport_);
}

glm::vec3 Camera::WorldToCamera(const glm::vec3& point) {
    return view_ * point;
}

glm::vec3 Camera::CameraToWorld(const glm::vec3& point) {
    return transform_ * point;
}

const glm::mat4x4& Camera::transform() const {
    return transform_;
}

void Camera::set_transform(const glm::mat4x4& transform) {
    transform_ = transform;
}

const glm::mat4x4& Camera::projection() const {
    return projection_;
}

int Camera::screen_height() const {
    return screen_height_;
}

int Camera::screen_width() const {
    return screen_width_;
}

const glm::mat4x4& Camera::unprojection() const {
    return unprojection_;
}

const glm::mat4x4& Camera::view() const {
    return view_;
}
} // namespace ray

