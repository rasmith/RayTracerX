/*
 * grid.cpp
 *
 *  Created on: Apr 1, 2014
 *      Author: agrippa
 */
#include "grid.hpp"
#include "shape.hpp"
#include "types.hpp"
namespace ray {
glm::ivec3 GridBase::OrientedIndex(const glm::ivec3& i, const glm::ivec3& o,
    const glm::ivec3& size) {
  return (size + i * o - ((1 - o) >> 1)) % size;
}

glm::ivec3 GridBase::Step(const glm::ivec3& index, const glm::ivec3& size) {
  glm::ivec3 result = index;
  ++result[0];
  result[1] += (result[0] == size[0]);
  result[2] += (result[1] == size[1]);
  result = result % size;
  return result;
}

UniformGridSampler::UniformGridSampler() :
    size_(glm::ivec3(2)), bounds_(), real_step_(ComputeRealStep()) {
}

UniformGridSampler::UniformGridSampler(glm::ivec3 size,
    const BoundingBox& bounds) :
    size_(size), bounds_(bounds), real_step_(ComputeRealStep()),
        num_cells_((size_[0] - 1) * (size_[1] - 1) * (size_[2] - 1)),
        num_vertices_(size_[0] * size_[1] * size_[2]) {
}

UniformGridSampler::UniformGridSampler(const UniformGridSampler& sg) :
    size_(sg.size_), bounds_(sg.bounds_), real_step_(sg.real_step_),
        num_cells_(sg.num_cells_), num_vertices_(sg.num_vertices_) {
}

const glm::ivec3& UniformGridSampler::size() const {
  return size_;
}

const BoundingBox& UniformGridSampler::bounds() const {
  return bounds_;
}

const glm::vec3& UniformGridSampler::real_step() const {
  return real_step_;
}

void UniformGridSampler::set_real_step(const glm::vec3& real_step) {
  real_step_ = real_step;
}

void UniformGridSampler::set_bounds(const BoundingBox& bounds) {
  bounds_ = bounds;
}

BoundingBox UniformGridSampler::GetBoundsAt(const glm::ivec3& index) {
  return BoundingBox(real_step_ * index, real_step_ * (index + 1));
}

glm::vec3 UniformGridSampler::GetVertexAt(const glm::ivec3& index) {
  return real_step_ * index + bounds_.min();
}

glm::vec3 UniformGridSampler::ComputeRealStep() {
  glm::vec3 real_step = glm::vec3(0.0f);
  glm::vec3 inv = glm::vec3(1.0f / (size_[0] - 1), 1.0f / (size_[1] - 1),
      1.0f / (size_[2] - 1));
  real_step = inv * (bounds_.max() - bounds_.min());
  return real_step;
}

bool UniformGridSampler::PointToCellIndex(const glm::vec3& point,
    glm::ivec3& index) {
  if (!bounds_.Contains(point))
    return false;
  index = glm::floor((point - bounds_.min()) / real_step_);
  index = glm::clamp(index, glm::ivec3(0), size_ - 2);
  return true;
}

int UniformGridSampler::num_cells() const {
  return num_cells_;
}

int UniformGridSampler::num_vertices() const {
  return num_vertices_;
}

glm::ivec3 UniformGridSampler::StepCell(const glm::ivec3& index) const {
  glm::ivec3 result = index;
  ++result[0];
  result[1] += (result[0] == size_[0] - 1);
  result[2] += (result[1] == size_[1] - 1);
  result = result % (size_ - 1);
  return result;
}

glm::ivec3 UniformGridSampler::StepVertex(const glm::ivec3& index) const {
  glm::ivec3 result = index;
  ++result[0];
  result[1] += (result[0] == size_[0]);
  result[2] += (result[1] == size_[1]);
  result = result % size_;
  return result;
}
}

