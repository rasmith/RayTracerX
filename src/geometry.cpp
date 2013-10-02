/*
 * geom.cpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */

#include <cstring>
#include "geometry.hpp"
namespace ray {
namespace geom {
Triangle::Triangle() {
}

Triangle::Triangle(const Triangle& triangle) {
    memcpy(vertices, triangle.vertices, sizeof(glm::vec3) * 3);
}

Triangle::Triangle(const glm::vec3& a, const glm::vec3& b,
        const glm::vec3& c) {
    vertices[0] = a;
    vertices[1] = b;
    vertices[2] = c;
}
} // namespace geom
} // namespace ray
