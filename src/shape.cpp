/*
 * shape.cpp
 *
 *  Created on: Nov 25, 2013
 *      Author: agrippa
 */
#include <ostream>
#include "io_utils.hpp"
#include "shape.hpp"
namespace ray {
BoundingBox::BoundingBox() :
    min_(glm::vec3(std::numeric_limits<float>::max())),
        max_(glm::vec3(-std::numeric_limits<float>::max())) {
}

BoundingBox::BoundingBox(const glm::vec3& min_extents,
    const glm::vec3& max_extents) :
    min_(min_extents), max_(max_extents) {
}

BoundingBox::BoundingBox(const BoundingBox& bbox) :
    min_(bbox.min_), max_(bbox.max_) {
}

const glm::vec3& BoundingBox::max() const {
  return max_;
}

const glm::vec3& BoundingBox::min() const {
  return min_;
}

glm::vec3& BoundingBox::max() {
  return max_;
}

glm::vec3& BoundingBox::min() {
  return min_;
}

void BoundingBox::set_min(const glm::vec3& min_extents) {
  min_ = min_extents;
}

void BoundingBox::set_max(const glm::vec3& max_extents) {
  max_ = max_extents;
}

BoundingBox BoundingBox::Join(const BoundingBox& bbox) const {
  BoundingBox result;
  result.min_ = glm::min(min_, bbox.min_);
  result.max_ = glm::max(max_, bbox.max_);
  return result;
}

glm::vec3 BoundingBox::GetCenter() const {
  return 0.5f * (min_ + max_);
}

// Kay/Kajiya slabs algorithm based off PBRTv2 pp 194-195
// Need IEEE floating point arithmetic for this to work
bool BoundingBox::Intersect(const Ray& ray, float& t_near, float& t_far) const {
  float t_min = -std::numeric_limits<float>::max();
  float t_max = std::numeric_limits<float>::max();
  bool hit = true;
  for (int i = 0; i < 3 && hit; ++i) {
    float inv = 1.0f / ray.direction()[i];
    float t_first = (min_[i] - ray.origin()[i]) * inv;
    float t_second = (max_[i] - ray.origin()[i]) * inv;
    if (t_first > t_second)
      std::swap(t_first, t_second);
    t_min = std::max(t_min, t_first);
    t_max = std::min(t_max, t_second);
    hit = (t_min <= t_max);
  }
  if (t_near)
    t_near = t_min;
  if (t_far)
    t_far = t_max;
  std::cout << "BoundingBox::Intersect: ray = " << ray << " t_near = " << t_near
      << " t_far = " << t_far << " hit = " << hit << std::endl;
  return t_near < t_far;
}

bool BoundingBox::Overlap(const BoundingBox& bbox) const {
  //std::cout << "this = " << *this << " bbox = " << bbox << "\n";
  bool overlap = true;
  for (uint32_t i = 0; i < 3; ++i) {
    overlap = overlap && (min_[i] <= bbox.max()[i] && bbox.min()[i] <= max_[i]);
  }
  return overlap;
}

bool BoundingBox::operator==(const BoundingBox& bbox) const {
  return min_ == bbox.min() && max_ == bbox.max();
}

BoundingBox& BoundingBox::operator =(const BoundingBox& bbox) {
  if (this == &bbox)
    return *this;
  min_ = bbox.min_;
  max_ = bbox.max_;
  return *this;
}

std::ostream& operator<<(std::ostream& out, const BoundingBox& b) {
  std::cout << "bbox: min = " << b.min() << " " << " max = " << b.max();
  return out;
}

Shape::~Shape() {
}

void Shape::Print(std::ostream& out) const {
  out << "[Shape]";
}
std::ostream& operator<<(std::ostream& out, const Shape& s) {
  s.Print(out);
  return out;
}

Isect::Isect() :
    obj(NULL), mat(NULL), ray(), normal(glm::vec3(0.0f)), bary(glm::vec3(0.0f)),
        uv(glm::vec3(0.0f)), t_hit(-std::numeric_limits<float>::max()) {
}

bool Isect::operator ==(const Isect& isect) const {
  return mat == isect.mat && normal == isect.normal && obj == isect.obj
      && bary == isect.bary && ray == isect.ray && t_hit == isect.t_hit
      && uv == isect.uv;
}

std::ostream& operator <<(std::ostream& out, const Isect& isect) {
  out << "[Isect bary = " << isect.bary << " mat = ";
  if (isect.mat)
    out << *(isect.mat);
  else
    out << "NULL";
  out << " n = " << isect.normal << " obj = ";
  if (isect.obj)
    out << *(isect.obj);
  else
    out << "NULL";
  out << " r = " << isect.ray << " t = " << isect.t_hit << " uv = " << isect.uv
      << "]";
  return out;
}
} // namespace ray
