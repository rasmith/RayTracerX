/*
 * geometry.hpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */
#ifndef GEOMETRY_HPP_
#define GEOMETRY_HPP_
#include "types.hpp"
#include "material.hpp"
namespace ray {
struct Triangle {
    Triangle();
    Triangle(const Triangle& triangle);
    Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
    glm::vec3 vertices[3];
};
struct Isect {
	Triangle obj;
	Material mat;
	Ray ray;
	glm::vec3 normal;
	glm::vec3 bary;
	glm::vec2 uv;
	float t_hit;
};

} // namespace ray
#endif /* GEOMETRY_HPP_ */
