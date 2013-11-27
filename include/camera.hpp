/*
 * camera.hpp
 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */

#ifndef CAMERA_HPP_
#define CAMERA_HPP_
#include "ray.hpp"
#include "types.hpp"
namespace ray {
class Camera {
public:
    Camera();
    Camera(const Camera& cam);
    Camera(float screen_width, float screen_height,
            const glm::mat4x4& projection,
            const glm::mat4x4& look_at);
    // To use Project(), the point has to be in world coordinates,
    // so remember to transform points from object to world space first,
    // if necessary.
    glm::vec3 Project(const glm::vec3& point); // world to screen
    // The point returned by UnProject() will be in world coordinates,
    // so remember to transform to object coordinates, if necessary.
    glm::vec3 UnProject(const glm::vec3& point); // screen to world
    glm::vec3 WorldToCamera(const glm::vec3& point); // world to camera
    glm::vec3 CameraToWorld(const glm::vec3& point); // camera to world
    void Resize(int width, int height); // resize screen width / height
    Ray GenerateRay(float screen_x, float screen_y) const;
    const glm::mat4x4& transform() const;
    const glm::mat4x4& projection() const;
    int screen_height() const;
    int screen_width() const;
    const glm::mat4x4& unprojection() const;
    const glm::mat4x4& view() const;
    Camera& operator=(const Camera& cam);
    const glm::vec4& direction() const;
private:
    int screen_width_;
    int screen_height_;
    glm::vec4 viewport_;
    glm::mat4x4 view_;
    glm::mat4x4 transform_;
    glm::mat4x4 projection_;
    glm::mat4x4 unprojection_;
    float focal_length_;
    glm::vec4 up_;
    glm::vec4 direction_;
    glm::vec4 tangent_;
    glm::vec4 center_;
    glm::vec4 screen_center_;

};
} // namespace ray
#endif /* CAMERA_HPP_ */
