/*
 * geom.cpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <numeric>

#include "geometry.hpp"
#include "io_utils.hpp"
#include "ray.hpp"
namespace ray {
const float Triangle::kEpsilon = 2e-10;

Triangle::Triangle() :
    Shape() {
}

Triangle::Triangle(const Triangle& triangle) :
    Shape() {
  memcpy(vertices_, triangle.vertices_, sizeof(glm::vec3) * 3);
}

Triangle::Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) :
    Shape() {
  vertices_[0] = a;
  vertices_[1] = b;
  vertices_[2] = c;
}

glm::vec3& Triangle::operator[](int i) {
  return vertices_[i];
}

const glm::vec3& Triangle::operator[](int i) const {
  return vertices_[i];
}

/**
 * Moller-Trumbore triangle intersection test:
 *  Fast, Minimum Storage Ray/Triangle Intersection.
 *  Tomas Moller and Ben Trumbore.
 *  Journal of Graphics Tools, 2(1):21--28, 1997
 **/
bool Triangle::Intersect(const Ray& ray, Isect& isect) const {
  glm::vec3 a = vertices_[0];
  glm::vec3 b = vertices_[1];
  glm::vec3 c = vertices_[2];
  glm::vec3 e1 = b - a;
  glm::vec3 e2 = c - a;
  glm::vec3 p_vec = glm::cross(ray.direction(), e2);
  float det = glm::dot(e1, p_vec);
  if (det > -Triangle::kEpsilon && det < Triangle::kEpsilon)
    return false;
  float inv_det = 1.0f / det;
  glm::vec3 t_vec = ray.origin() - a;
  glm::vec3 q_vec = glm::cross(t_vec, e1);
  float t = glm::dot(e2, q_vec) * inv_det;
  // Do not allow ray origin in front of triangle
  if (t < 0.0f)
    return false;
  float u = glm::dot(t_vec, p_vec) * inv_det;
  if (u < 0.0f || u > 1.0f)
    return false;
  float v = glm::dot(ray.direction(), q_vec) * inv_det;
  if (v < 0.0f || u + v > 1.0f)
    return false;

  isect.t_hit = t;
  isect.bary = glm::vec3(1.0f - u - v, u, v);
  isect.ray = ray;
  isect.normal = glm::normalize(glm::cross(e1, e2));
  isect.obj = static_cast<const Shape*>(this);
  return true;
}

glm::vec3 Triangle::GetNormal() {
  glm::vec3 a = vertices_[0];
  glm::vec3 b = vertices_[1];
  glm::vec3 c = vertices_[2];
  glm::vec3 e1 = glm::normalize(b - a);
  glm::vec3 e2 = glm::normalize(c - a);
  return glm::normalize(glm::cross(e1, e2));
}

BoundingBox Triangle::GetBounds() {
  glm::vec3 min = glm::min(glm::min(vertices_[0], vertices_[1]), vertices_[2]);
  glm::vec3 max = glm::max(glm::max(vertices_[0], vertices_[1]), vertices_[2]);
  return BoundingBox(min, max);
}

Sphere::Sphere() :
    center_(glm::vec3(0.0f)), radius_(0.0f) {
}

Sphere::Sphere(const Sphere& sphere) :
    center_(sphere.center_), radius_(sphere.radius_) {
}

Sphere::Sphere(const glm::vec3& center, float radius) :
    center_(center), radius_(radius) {
}

bool Sphere::Intersect(const Ray& ray, Isect& isect) const {
  bool hit = false;
  glm::vec3 O = ray.origin() - center_;
  glm::vec3 V = ray.direction();
  float dotOV = glm::dot(O, V);
  float dotOO = glm::dot(O, O);
  float dotVV = glm::dot(V, V);
  float r = radius_;
  float discrim = dotOV * dotOV - dotVV * (dotOO - r * r);
  if (discrim < 0.0f) {
    return false;
  }
  float s = sqrt(discrim);
  float num0 = (-dotOV + s) / dotVV;
  float num1 = (-dotOV - s) / dotVV;
  hit = (num0 > 0.0f || num1 > 0.0f);
  if (hit) {
    isect.t_hit = (
        num0 > 0.0f && num1 > 0.0f ? std::min(num0, num1) :
                                     (num0 > 0.0f ? num0 : num1));
    isect.normal = glm::normalize(ray(isect.t_hit) - center_);
    isect.ray = ray;
  }
  return hit;
}

BoundingBox Sphere::GetBounds() {
  return BoundingBox(center_ + radius_, center_ - radius_);
}
} // namespace ray

