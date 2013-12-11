/*
 * shape.hpp
 *
 *  Created on: Nov 25, 2013
 *      Author: agrippa
 */

#ifndef SHAPE_HPP_
#define SHAPE_HPP_
#include <ostream>
#include "io_utils.hpp"
#include "material.hpp"
#include "ray.hpp"
namespace ray {
class BoundingBox {
public:
    BoundingBox();
    BoundingBox(const glm::vec3& min_extents, const glm::vec3& max_extents);
    BoundingBox(const BoundingBox& bbox);
    void set_min(const glm::vec3& min_extents);
    void set_max(const glm::vec3& max_extents);
    const glm::vec3& max() const;
    const glm::vec3& min() const;
    BoundingBox Join(const BoundingBox& bbox);
private:
    glm::vec3 min_;
    glm::vec3 max_;
};

struct Isect;

class Shape {
public:
    virtual ~Shape();
    virtual bool Intersect(const Ray& ray, Isect& isect) const = 0;
    virtual BoundingBox GetBounds() = 0;
    virtual void Print(std::ostream& out) const;
};
std::ostream& operator<<(std::ostream& out, const Shape& scene);
struct Isect {
    Shape* obj;
    Material* mat;
    Ray ray;
    glm::vec3 normal;
    glm::vec3 bary;
    glm::vec2 uv;
    float t_hit;
};
} // namespace ray
#endif /* SHAPE_HPP_ */
