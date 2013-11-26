/*
 * geom.cpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */

#include <numeric>
#include <cstring>
#include "geometry.hpp"
#include "ray.hpp"
namespace ray {
const float Triangle::kEpsilon = 0.000001;

Triangle::Triangle() {
}

Triangle::Triangle(const Triangle& triangle) {
    memcpy(vertices_, triangle.vertices_, sizeof(glm::vec3) * 3);
}

Triangle::Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
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
 * Möller-Trumbore triangle intersection test:
 *  Fast, Minimum Storage Ray/Triangle Intersection.
 *  Tomas Möller and Ben Trumbore.
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
    if (det > -Triangle::kEpsilon && det < Triangle::kEpsilon) {
        return false;
    }
    float inv_det = 1.0f / det;
    glm::vec3 t_vec = ray.origin() - a;
    glm::vec3 q_vec = glm::cross(t_vec, e1);
    float t = glm::dot(e2, q_vec) * inv_det;
    // Do not allow ray origin in front of triangle
    if (t < 0.0f) {
        return false;
    }
    float u = glm::dot(t_vec, p_vec) * inv_det;
    if (u < 0.0f || u > 1.0f) {
        return false;
    }
    float v = glm::dot(ray.direction(), q_vec) * inv_det;
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }
    isect.t_hit = t;
    isect.bary = glm::vec3(u, v, 1.0f - u - v);
    isect.ray = ray;
    return true;
}

BoundingBox Triangle::GetBounds() {
    glm::vec3 min = glm::min(glm::min(vertices_[0], vertices_[1]),
            vertices_[2]);
    glm::vec3 max = glm::max(glm::max(vertices_[0], vertices_[1]),
            vertices_[2]);
    return BoundingBox(min, max);
}
} // namespace ray

