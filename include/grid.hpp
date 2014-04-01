/*
 * grid.hpp
 *
 *  Created on: Apr 1, 2014
 *      Author: agrippa
 */

#ifndef GRID_HPP_
#define GRID_HPP_
#include <cstdlib>
namespace ray {
template<class ValueType>
class Grid {
public:
  Grid() :
      size_(glm::ivec3(1)), grid_size_(0), values_(NULL) {
  }

  explicit Grid(const glm::ivec3& size) :
      size_(size), grid_size_(0), values_(NULL) {
  }

  Grid(const Grid& g) :
      size_(glm::ivec3(0)), grid_size_(0), values_(NULL) {
    Copy(g);
  }

  virtual ~Grid() {
    if (values_)
      delete[] values_;
  }

  ValueType& operator()(int i, int j, int k) {
    return values_[i * size_[0] * size_[1] + j * size_[1] + k];
  }

  const ValueType& operator()(int i, int j, int k) const {
    return values_[i * size_[0] * size_[1] + j * size_[1] + k];
  }

  ValueType& operator()(const glm::ivec3& index) {
    return values_[index[0] * size_[0] * size_[1] + index[1] * size_[1]
        + index[2]];
  }

  const ValueType& operator()(const glm::ivec3& index) const {
    return values_[index[0] * size_[0] * size_[1] + index[1] * size_[1]
        + index[2]];
  }

  const glm::ivec3& size() const {
    return size_;
  }

  void set_size(const glm::ivec3& size) {
    size_ = size;
  }

  int grid_size() const {
    return grid_size_;
  }

  void AssignToAll(const ValueType& v) {
    glm::ivec3 index(0);
    for (int n = 0; n < grid_size(); ++n) {
      (*this)(index) = v;
      index = Step(index);
    }
  }

  glm::ivec3 OrientedPrevious(glm::ivec3& index, const glm::ivec3& orientation,
      int d) {
    glm::ivec3 result = index;
    int i = (size_[d] + index[d] + orientation[d]) % size_[d];
    result[d] = i;
    return result;
  }

  glm::ivec3 OrientedStep(const glm::ivec3& index,
      const glm::ivec3& orientation) {
    glm::ivec3 result = (size_ + orientation * index) % size_;
    ++result[0];
    result[1] += (result[0] == size_[0]);
    result[2] += (result[1] == size_[1]);
    result = (size_ + orientation * result) % size_;
    return result;
  }

  glm::ivec3 Step(const glm::ivec3& index) const {
    glm::ivec3 result = index;
    ++result[0];
    result[1] += (result[0] == size_[0]);
    result[2] += (result[1] == size_[1]);
    result = result % size_;
    return result;
  }

  void Init() {
    if (values_)
      delete[] values_;
    grid_size_ = size_[0] * size_[1] * size_[2];
    values_ = new ValueType[grid_size_];
    for (int i = 0; i < grid_size_; ++i) {
      values_[i] = ValueType();
    }
  }

  void Copy(const Grid& g) {
    size_ = g.size_;
    grid_size_ = size_[0] * size_[1] * size_[2];
    if (!values_)
      values_ = new ValueType[grid_size_];
    else
      delete[] values_;
    for (int i = 0; i < grid_size_; ++i) {
      values_[i] = g.values_[i];
    }
  }

  friend std::ostream& operator<<(std::ostream& out, const Grid<ValueType>& g) {
    glm::ivec3 index(0);
    for (int n = 0; n < g.grid_size(); ++n) {
      out << g(index);
      if (index[0] == g.size_[0] - 1)
        out << "\n";
      if (index[0] == g.size_[0] - 1 && index[2] == g.size_[0] - 1)
        out << "\n";
      index = g.Step(index);
    }
    return out;
  }
protected:
  glm::ivec3 size_;
  int grid_size_;
  ValueType* values_;
};

template<class ValueType>
class SummableGrid: public Grid<ValueType> {
public:
  SummableGrid() :
      Grid<ValueType>::Grid() {
  }

  SummableGrid(const SummableGrid& g) :
      Grid<ValueType>::Grid(g) {
  }

  explicit SummableGrid(const glm::ivec3& size) :
      Grid<ValueType>::Grid(size) {
  }

  virtual ~SummableGrid() {
  }

  void OrientedIntegralImage(const glm::ivec3& orientation) {
    glm::ivec3 index((this->size_ + orientation) % this->size_);
    glm::ivec3 previous(0);
    for (int d = 0; d < 3; ++d) {
      for (int n = 0; n < this->grid_size(); ++n) {
        if ((this->size_[d] + index[d] * orientation[d]) % this->size_[d] > 0) {
          previous = this->OrientedPrevious(index, orientation, d);
          --previous[d];
          (*this)(index) += (*this)(previous);
        }
        index = this->OrientedStep(index, orientation);
      }
    }
  }

  void IntegralImage() {
    glm::ivec3 index(0);
    glm::ivec3 previous(0);
    for (int d = 0; d < 3; ++d) {
      for (int n = 0; n < this->grid_size(); ++n) {
        if (index[d] > 0) {
          previous = index;
          --previous[d];
          (*this)(index) += (*this)(previous);
        }
        index = this->Step(index);
      }
    }
  }
};

/**
 * UniformGridSampler
 *
 * size = N x M x L = dimensionality w.r.t. to the vertices
 * with (N - 1) x (M - 1) x (L - 1) cells
 *
 */
class UniformGridSampler {
public:
  UniformGridSampler();
  explicit UniformGridSampler(glm::ivec3 size, const BoundingBox& bounds);
  explicit UniformGridSampler(const UniformGridSampler& sg);
  const BoundingBox& bounds() const;
  const glm::vec3& real_step() const;
  void set_real_step(const glm::vec3& real_step);
  void set_bounds(const BoundingBox& bounds);
  BoundingBox GetBoundsAt(const glm::ivec3& index);
  glm::vec3 GetVertexAt(const glm::ivec3& index);
protected:
  glm::vec3 ComputeRealStep();
  glm::ivec3 size_;
  BoundingBox bounds_;
  glm::vec3 real_step_;
};
}

#endif /* GRID_HPP_ */
