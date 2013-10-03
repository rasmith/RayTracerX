/*
 * geometry.hpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */
#ifndef GEOMETRY_HPP_
#define GEOMETRY_HPP_
#include "types.hpp"
namespace ray {
struct Triangle {
    Triangle();
    Triangle(const Triangle& triangle);
    Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
    glm::vec3 vertices[3];
};
} // namespace ray
#endif /* GEOMETRY_HPP_ */
