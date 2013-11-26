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
#include "shape.hpp"
#include "types.hpp"
namespace ray {
class Triangle: public Shape {
public:
    Triangle();
    Triangle(const Triangle& triangle);
    Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
    glm::vec3& operator[](int i);
    const glm::vec3& operator[](int i) const;
    bool Intersect(const Ray& ray, Isect& isect) const;
    BoundingBox GetBounds();
private:
    static const float kEpsilon;
    glm::vec3 vertices_[3];
};

class Sphere: public Shape {
public:
    Sphere();
    Sphere(const Sphere& sphere);
    Sphere(glm::vec3& center, float radius);
    bool Intersect(const Ray& ray, Isect& isect) const;
    BoundingBox GetBounds();
private:
    glm::vec3 center_;
    float radius_;
};

} // namespace ray
#endif /* GEOMETRY_HPP_ */
