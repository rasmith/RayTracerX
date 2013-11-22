/*
 * geom.cpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */

#include <cstring>
#include "geometry.hpp"
#include "ray.hpp"
namespace ray {
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

bool Triangle::Intersect(const Ray& ray, Isect& isect) const {

}
} // namespace ray
