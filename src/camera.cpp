/*
 * camera.cpp
 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */
#include <ostream>
#include "camera.hpp"
#include "io_utils.hpp"
#include "ray.hpp"
#include "transform.hpp"
namespace ray {
Camera::Camera() :
        screen_width_(640.0f), screen_height_(480.0f),
                viewport_(glm::vec4(0.0f, 0.0f, screen_width_, screen_height_)),
                view_(
                        LookAt(glm::vec3(0.0f, 0.0f, 0.0f),
                                glm::vec3(0.0f, 0.0f, -1.0f),
                                glm::vec3(0.0f, 1.0f, 0.0f))),
                transform_(Inverse(view_)),
                projection_(Orthographic(0.0f, 1.0f)),
                unprojection_(Inverse(projection_)), focal_length_(0.0f),
                up_(
                        glm::normalize(
                                transform_
                                        * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f))),
                direction_(
                        glm::normalize(
                                transform_
                                        * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f))),
                tangent_(
                        glm::normalize(
                                transform_
                                        * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f))),
                center_(transform_ * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
                screen_center_(center_ + direction_ * focal_length_) {
}

Camera::Camera(const Camera& cam) :
        screen_width_(cam.screen_width_), screen_height_(cam.screen_height_),
                viewport_(cam.viewport_), view_(cam.view_),
                transform_(cam.transform_), projection_(cam.projection_),
                unprojection_(cam.unprojection_),
                focal_length_(cam.focal_length_), up_(cam.up_),
                direction_(cam.direction_), tangent_(cam.tangent_),
                center_(cam.center_), screen_center_(cam.screen_center_) {
}

Camera::Camera(
        float screen_width,
        float screen_height,
        const glm::mat4x4& projection,
        const glm::mat4x4& look_at) :
        screen_width_(screen_width), screen_height_(screen_height),
                viewport_(glm::vec4(0.0f, 0.0f, screen_width_, screen_height_)),
                view_(look_at), transform_(Inverse(look_at)),
                projection_(projection), unprojection_(Inverse(projection_)),
                focal_length_(1.0f),
                up_(
                        glm::normalize(
                                transform_
                                        * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f))),
                direction_(
                        glm::normalize(
                                transform_
                                        * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f))),
                tangent_(
                        glm::normalize(
                                transform_
                                        * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f))),
                center_(transform_ * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
                screen_center_(center_ + direction_ * focal_length_) {
}

glm::vec3 Camera::Project(const glm::vec3& point) {
    return glm::project(point, view_, projection_, viewport_);
}

glm::vec3 Camera::UnProject(const glm::vec3& point) {
    return glm::unProject(point, view_, projection_, viewport_);
}

glm::vec3 Camera::WorldToCamera(const glm::vec3& point) {
    return glm::vec3(view_ * glm::vec4(point, 1.0f));
}

glm::vec3 Camera::CameraToWorld(const glm::vec3& point) {
    return glm::vec3(transform_ * glm::vec4(point, 1.0f));
}

void Camera::Resize(int width, int height) {
    *this = Camera(width, height, projection_, view_);
}

const glm::mat4x4& Camera::transform() const {
    return transform_;
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

Ray Camera::GenerateRay(float screen_x, float screen_y) const {
    float u = screen_x / static_cast<float>(screen_width_ - 1) - 0.5f;
    float v = (screen_height_ - screen_y)
            / static_cast<float>(screen_height_ - 1) - 0.5f;
    //glm::vec4 origin = screen_center_ + u * tangent_ + v * up_;
    //glm::vec4 direction = screen_center_ + u * tangent_ + v * up_ - center_;
    glm::vec4 origin = center_;
    glm::vec4 direction = glm::normalize(direction_ + u * tangent_ + v * up_);
    return Ray(glm::vec3(origin[0], origin[1], origin[2]),
            glm::vec3(direction[0], direction[1], direction[2]));
}

Camera& Camera::operator=(const Camera& cam) {
    if (this == &cam) {
        return *this;
    }
    screen_width_ = cam.screen_width_;
    screen_height_ = cam.screen_height_;
    viewport_ = cam.viewport_;
    view_ = cam.view_;
    transform_ = cam.transform_;
    projection_ = cam.projection_;
    unprojection_ = cam.unprojection_;
    up_ = cam.up_;
    direction_ = cam.direction_;
    tangent_ = cam.tangent_;
    center_ = cam.center_;
    screen_center_ = cam.screen_center_;
    return *this;
}

const glm::vec4& Camera::direction() const {
    return direction_;
}
std::ostream& operator<<(std::ostream& out, const Camera& c) {
    out << "[";
    out << "M: " << c.transform() << ",";
    out << "V: " << c.view() << ",";
    out << "d: " << c.direction() << ",";
    out << "]";
    return out;
}
} // namespace ray
