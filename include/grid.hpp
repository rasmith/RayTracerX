/*
 * grid.hpp
 *
 *  Created on: Apr 1, 2014
 *      Author: agrippa
 */

#ifndef GRID_HPP_
#define GRID_HPP_
#include <cstdlib>
#include "shape.hpp"
#include "types.hpp"
namespace ray {

class GridBase {
public:
  static glm::ivec3 OrientedIndex(const glm::ivec3& i, const glm::ivec3& o,
      const glm::ivec3& size);

  static glm::ivec3 Step(const glm::ivec3& index, const glm::ivec3& size);
};

template<class ValueType>
class Grid: public GridBase {
public:
  Grid() :
      size_(glm::ivec3(1)), grid_size_(0), values_(NULL) {
  }

  explicit Grid(const glm::ivec3& size) :
      size_(size), grid_size_(0), values_(NULL) {
  }

  explicit Grid(int i, int j, int k) :
      size_(glm::ivec3(i, j, k)), grid_size_(0), values_(NULL) {
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
    return values_[ValueIndex(i, j, k)];
  }

  const ValueType& operator()(int i, int j, int k) const {
    return values_[ValueIndex(i, j, k)];
  }

  ValueType& operator()(const glm::ivec3& index) {
    return values_[ValueIndex(index)];
  }

  ValueType& operator[](unsigned int i) {
    return values_[i];
  }

  const ValueType& operator[](unsigned int i) const {
    return values_[i];
  }

  const ValueType& operator()(const glm::ivec3& index) const {
    return values_[ValueIndex(index)];
  }

  bool IsSafe(int i, int j, int k) const {
    return ((i >= 0 && i < this->size_[0]) && (j >= 0 && j < this->size_[1])
        && (k >= 0 && k < this->size_[2]));
  }

  const ValueType& GetSafe(int i, int j, int k, const ValueType& v) const {
    return (IsSafe(i, j, k) ? (*this)(i, j, k) : v);
  }

  const ValueType& GetSafe(const glm::ivec3& i, const ValueType& v) const {
    return (IsSafe(i[0], i[1], i[2]) ? (*this)(i[0], i[1], i[2]) : v);
  }

  Grid<ValueType>& operator=(const Grid<ValueType>& g) {
    Copy(g);
    return *this;
  }

  bool operator==(const Grid<ValueType>& g) const {
    if (size_ != g.size_)
      return false;
    bool equals = true;
    for (int i = 0; i < grid_size_ && equals; ++i)
      equals = (values_[i] == g.values_[i]);
    return equals;
  }

  bool operator!=(const Grid<ValueType>& g) const {
    return !(*this == g);
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
    for (int n = 0; n < grid_size(); ++n)
      values_[n] = v;
  }

  glm::ivec3 OrientedIndex(const glm::ivec3& i, const glm::ivec3& o) {
    return (size_ + i * o - ((1 - o) >> 1)) % size_;
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
    for (int i = 0; i < grid_size_; ++i)
      values_[i] = ValueType();
  }

  void Copy(const Grid<ValueType>& g) {
    size_ = g.size_;
    grid_size_ = size_[0] * size_[1] * size_[2];
    if (values_)
      delete[] values_;
    values_ = new ValueType[grid_size_];
    for (int n = 0; n < grid_size_; ++n)
      values_[n] = g.values_[n];
  }

  friend std::ostream& operator<<(std::ostream& out, const Grid<ValueType>& g) {
    glm::ivec3 index(0);
    for (int n = 0; n < g.grid_size(); ++n) {
      out << g(index);
      if (index[0] == g.size_[0] - 1)
        out << "\n";
      else
        out << " ";
      if (index[0] == g.size_[0] - 1 && index[1] == g.size_[0] - 1)
        out << "\n";
      index = g.Step(index);
    }
    return out;
  }
protected:
  inline int ValueIndex(int i, int j, int k) const {
    return i + j * size_[0] + k * size_[0] * size_[1];
  }
  inline int ValueIndex(const glm::ivec3 i) const {
    return ValueIndex(i[0], i[1], i[2]);
  }
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

  void OrientedImageIntegral(const glm::ivec3& o) {
    glm::ivec3 index(0);
    glm::ivec3 previous(0);
    for (int d = 0; d < 3; ++d) {
      for (int n = 0; n < this->grid_size(); ++n) {
        if (index[d] > 0) {
          previous = index;
          --previous[d];
          (*this)(this->OrientedIndex(index, o)) += (*this)(
              this->OrientedIndex(previous, o));
        }
        index = this->Step(index);
      }
    }
  }

  void ImageIntegral() {
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
  const glm::ivec3& size() const;
  const BoundingBox& bounds() const;
  const glm::vec3& real_step() const;
  int num_cells() const;
  int num_vertices() const;
  void set_real_step(const glm::vec3& real_step);
  void set_bounds(const BoundingBox& bounds);
  BoundingBox GetBoundsAt(const glm::ivec3& index);
  glm::vec3 GetVertexAt(const glm::ivec3& index);
  bool PointToCellIndex(const glm::vec3& point, glm::ivec3& index);
  glm::ivec3 StepCell(const glm::ivec3& index) const;
  glm::ivec3 StepVertex(const glm::ivec3& index) const;
protected:
  glm::vec3 ComputeRealStep();
  glm::ivec3 size_;
  BoundingBox bounds_;
  glm::vec3 real_step_;
  int num_cells_;
  int num_vertices_;
};
}

#endif /* GRID_HPP_ */
