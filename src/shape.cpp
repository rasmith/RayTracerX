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
		min_(glm::vec3(std::numeric_limits<float>::max())), max_(
				glm::vec3(-std::numeric_limits<float>::max())) {
}

BoundingBox::BoundingBox(const glm::vec3& min_extents,
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

BoundingBox BoundingBox::Join(const BoundingBox& bbox) const {
	BoundingBox result;
	result.min_ = glm::min(min_, bbox.min_);
	result.min_ = glm::max(max_, bbox.max_);
	return result;
}

glm::vec3 BoundingBox::GetCenter() const {
	return 0.5f * (min_ + max);
}

// Kay/Kajiya slabs algorithm based off PBRTv2 pp 194-195
// Need IEEE floating point arithmetic for this to work
bool BoundingBox::Intersect(const Ray& ray, float& t_near, float& t_far) const {
	float t_min = -std::numeric_limits<float>::max();
	float t_max = std::numeric_limits<float>::max();
	for (int i = 0; i < 3; ++i) {
		float inv = 1.0f / ray.direction()[i];
		float t_first = (min_[i] - ray.origin()[i]) * inv;
		float t_second = (max_[i] - ray.origin()) * inv;
		if (t_first > t_second)
			std::swap(t_first, t_second);
		t_min = std::max(t_min, t_first);
		t_max = std::min(t_max, t_second);
		if (t_min > t_max)
			return false;
	}
	if (t_near)
		t_near = t_min;
	if (t_far)
		t_far = t_max;
	return true;
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

bool BoundingBox::Overlap(const BoundingBox& bbox) const {
	bool overlap = true;
	for (uint32_t i = 0; i < 3; ++i) {
		float distance = fabs(bbox.max()[i] - max_[i]);
		float length1 = max_[i] - min_[i];
		float length2 = bbox.max()[i] - bbox.min()[i];
		overlap = overlap && (distance <= (length1 + length2));
	}
	return overlap;
}

} // namespace ray

