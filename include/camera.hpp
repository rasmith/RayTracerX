/*
 * camera.hpp
 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */

#ifndef CAMERA_HPP_
#define CAMERA_HPP_
#include "types.hpp"
namespace ray {
class Camera {
public:
    Camera();
    Camera(const Camera& cam);
    Camera(float near, float far, float aspect, float fov,
            const glm::vec3& at, const glm::vec3& position,
            const glm::vec3& up);
    glm::vec4 Project(const glm::vec3& point);
    glm::vec4 Unproject(const glm::vec3& point);

    // getters/setters
    float aspect() const;
    void set_aspect(float aspect);
    const glm::vec3& at() const;
    void set_at(const glm::vec3& at);
    float far() const;
    void set_far(float far);
    float fov() const;
    void set_fov(float fov);
    const std::string& name() const;
    void set_name(const std::string& name);
    float near() const;
    void set_near(float near);
    const glm::vec3& position() const;
    void set_position(const glm::vec3& position);
    const glm::mat4x4& transform() const;
    void set_transform(const glm::mat4x4& transform);
    const glm::vec3& up() const;
    void set_up(const glm::vec3& up);
    const glm::mat4x4& project() const;
    const glm::mat4x4& unproject() const;
    const glm::mat4x4& view() const;
private:
    float near_;
    float far_;
    float aspect_;
    float fov_;
    std::string name_;
    glm::vec3 at_;
    glm::vec3 position_;
    glm::vec3 up_;
    glm::mat4x4 transform_;
    glm::mat4x4 view_;
    glm::mat4x4 project_;
    glm::mat4x4 unproject_;
};
} // namespace ray
#endif /* CAMERA_HPP_ */
