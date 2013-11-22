/*
 * geometry.hpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */
#ifndef GEOMETRY_HPP_
#define GEOMETRY_HPP_
#include "material.hpp"
#include "ray.hpp"
#include "types.hpp"
namespace ray {
class Isect;

class Triangle {
public:
    Triangle();
    Triangle(const Triangle& triangle);
    Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
    glm::vec3& operator[](int i);
    const glm::vec3& operator[](int i) const;
    bool Intersect(const Ray& ray, Isect& isect) const;
private:
    static const float kEpsilon;
    glm::vec3 vertices_[3];
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
