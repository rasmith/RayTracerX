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

float BoundingBox::GetArea() const {
  glm::vec3 d = max_ - min_;
  return 2.0f * (d[0] * d[1] + d[0] * d[2] + d[1] * d[2]);
}

float BoundingBox::GetVolume() const {
  glm::vec3 d = max_ - min_;
  return d[0] * d[1] * d[2];
}

glm::vec3 BoundingBox::GetCenter() const {
  return 0.5f * (min_ + max_);
}

// Kay/Kajiya slabs algorithm based off PBRTv2 pp 194-195
// Need IEEE floating point arithmetic for this to work
bool BoundingBox::Intersect(const Ray& ray, float& t_near, float& t_far) const {
  //std::cout << "BoundingBox::Intersect ray = " << ray <<  std::endl;
  float t_min = -std::numeric_limits<float>::max();
  float t_max = std::numeric_limits<float>::max();
  for (int i = 0; i < 3; ++i) {
    float inv = 1.0f / ray.direction()[i];
    float t_a = (min_[i] - ray.origin()[i]) * inv;
    float t_b = (max_[i] - ray.origin()[i]) * inv;
    if (t_a > t_b)
      std::swap(t_a, t_b);
    t_min = std::max(t_min, t_a);
    t_max = std::min(t_max, t_b);
    if (t_min > t_max)
      return false;
  }
  t_near = t_min;
  t_far = t_max;
  //std::cout << "t_near = " << t_min << " t_far = " << t_max << std::endl;
  return t_near < t_far;
}

bool BoundingBox::Contains(const glm::vec3& point) const {
  bool contains = true;
  for (uint32_t i = 0; i < 3 && contains; ++i)
    contains = contains && (min_[i] <= point[i] && point[i] <= max_[i]);
  return contains;
}

bool BoundingBox::Overlap(const BoundingBox& bbox) const {
  bool overlap = true;
  for (uint32_t i = 0; i < 3 && overlap; ++i)
    overlap = overlap && (min_[i] <= bbox.max()[i] && bbox.min()[i] <= max_[i]);
  return overlap;
}

bool BoundingBox::Intersect(const BoundingBox& bbox, BoundingBox& out) const {
  bool overlap = true;
  for (uint32_t i = 0; i < 3 && overlap; ++i) {
    overlap = overlap && (min_[i] <= bbox.max()[i] && bbox.min()[i] <= max_[i]);
    out.min_[i] = std::max(min_[i], bbox.min_[i]);
    out.max_[i] = std::min(max_[i], bbox.max_[i]);
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
