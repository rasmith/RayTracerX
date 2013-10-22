/*
 * ray.cpp
 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */
#include "types.hpp"
#include "ray.hpp"
namespace ray {
Ray::Ray() : origin_(0.0f), direction_(0.0f) {
}

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction)
    : origin_(origin), direction_(direction) {
}

Ray::Ray(const Ray& ray)
    : origin_(ray.origin_), direction_(ray.direction_)  {
}

glm::vec3 Ray::operator ()(float t) const {
    return direction_ * t + origin_;
}

const glm::vec3& Ray::direction() const {
    return direction_;
}

const glm::vec3& Ray::origin() const {
    return origin_;
}
} // namespace ray


