/*
 * ray.cpp
 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */
#include <ostream>
#include "io_utils.hpp"
#include "types.hpp"
#include "ray.hpp"
namespace ray {
Ray::Ray() :
    origin_(0.0f), direction_(0.0f) {
}

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) :
    origin_(origin), direction_(direction) {
}

Ray::Ray(const Ray& ray) :
    origin_(ray.origin_), direction_(ray.direction_) {
}

Ray& ray::Ray::operator=(const Ray& ray) {
  if (this == &ray)
    return *this;
  origin_ = ray.origin_;
  direction_ = ray.direction_;
  return *this;
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

Ray Ray::operator-() const {
  return Ray(origin_, -direction_);
}

bool Ray::operator==(const Ray& r) const {
  return origin_ == r.origin_ && direction_ == r.direction_;
}

std::ostream& operator<<(std::ostream& out, const Ray& r) {
  out << "[Ray o = " << r.origin() << ", d = " << r.direction() << "]";
  return out;
}
} // namespace ray

