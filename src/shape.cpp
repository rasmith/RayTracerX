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

BoundingBox::BoundingBox(
        const glm::vec3& min_extents,
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

void BoundingBox::set_min(const glm::vec3& min_extents) {
    min_ = min_extents;
}

void BoundingBox::set_max(const glm::vec3& max_extents) {
    max_ = max_extents;
}

BoundingBox BoundingBox::Join(const BoundingBox& bbox) {
    BoundingBox result;
    result.min_ = glm::min(min_, bbox.min_);
    result.min_ = glm::max(max_, bbox.max_);
    return result;
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
} // namespace ray

