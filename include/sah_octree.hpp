/*
 * octree.hpp
 *
 *  Created on: Dec 19, 2013
 *      Author: agrippa
 */

#ifndef SAH_OCTREE_HPP_
#define SAH_OCTREE_HPP_
#include <sys/types.h>
#include <stdint.h>
#include <algorithm>
#include <cstring>
#include <limits>
#include <list>
#include <vector>
#include "grid.hpp"
#include "octree.hpp"
#include "octree_base.hpp"
#include "sah_octnode.hpp"
#include "shape.hpp"
namespace ray {
template<class SceneObject, int num_samples>
class SAHOctree: public Octree<SceneObject, SAHOctNode, SAHEncodedNode,
    SAHOctNodeFactory, 1, 32> {
public:
  typedef std::vector<const SceneObject*> ObjectVector;

  SAHOctree() :
          Octree<SceneObject, SAHOctNode, SAHEncodedNode, SAHOctNodeFactory, 1,
              32>::Octree() {
  }

  virtual ~SAHOctree() {
  }

  virtual uint32_t PointToOctant(const SAHOctNode& node, const BoundingBox&,
      const glm::vec3& point) const {
    glm::vec3 split = node.point();
    uint32_t x_bit = (point[0] > split[0]);
    uint32_t y_bit = (point[1] > split[1]);
    uint32_t z_bit = (point[2] > split[2]);
    uint32_t octant = x_bit | (y_bit << 1) | (z_bit << 2);
    return octant;
  }

  virtual BoundingBox GetOctantBounds(const glm::vec3& point,
      const BoundingBox& bounds, uint32_t octant) const {
    BoundingBox octant_bounds = bounds;
    for (uint32_t i = 0; i < 3; ++i) {
      if ((octant >> i) & 0x1)
        octant_bounds.min()[i] = split[i];
      else
        octant_bounds.max()[i] = split[i];
    }
    return octant_bounds;
  }

  virtual BoundingBox GetChildBounds(const SAHOctNode& node,
      const BoundingBox& bounds, uint32_t octant) const {
    glm::vec3 split = node.point();
    BoundingBox child_bounds = bounds;
    for (uint32_t i = 0; i < 3; ++i) {
      if ((octant >> i) & 0x1)
        child_bounds.min()[i] = split[i];
      else
        child_bounds.max()[i] = split[i];
    }
    return child_bounds;
  }

  virtual glm::ivec3 GetOctantBits(uint32_t octant) const {
    return glm::ivec3(octant & 0x1, (octant & 0x2) >> 1, (octant & 0x4) >> 2);
  }
protected:
  typedef typename Octree<SceneObject, SAHOctNode, SAHEncodedNode,
      SAHOctNodeFactory, 1, 32>::WorkNode WorkNodeType;

  typedef typename Octree<SceneObject, SAHOctNode, SAHEncodedNode,
      SAHOctNodeFactory, 1, 32>::WorkList WorkListType;

  float GetMinCost() const {
    return 1.0f;
  }

  void ComputeIntersections(const ObjectVector& objects,
      const BoundingBox& bounds, SummableGrid<int>& intersections) {
    glm::ivec3 index(0);
    BoundingBox object_bounds(0);
    UniformGridSampler sampler(intersections.size(), bounds);
    float curr_val = 0.0f, curr_step = 0.0f;
    glm::ivec3 min_index(0), max_index(0);
    for (int m = 0; m < objects.size(); ++m) {
      object_bounds = objects[i]->GetBounds();
      for (int d = 0; d < 3; ++d) {
        min_index[d] = 0;
        curr_val = object_bounds.min()[d];
        while (curr_val + curr_step < object_bounds.min()[d]
            && min_index[d] <= intersections.size()[d] - 1) {
          curr_val += curr_step;
          ++min_index[d];
        }
        max_index[d] = min_index[d];
        while (curr_val + curr_step < object_bounds.max()[d]
            && max_index[d] <= intersections.size()[d] - 1) {
          curr_val += curr_step;
          ++max_index[d];
        }
      }
      for (int i = min_index[0]; i <= max_index[0]; ++i)
        for (int j = min_index[1]; j <= max_index[1]; ++j)
          for (int k = min_index[2]; k <= max_index[2]; ++k)
            ++intersections(i, j, k);
    }
  }

  void ComputeImageIntegrals(const SummableGrid<int>& intersections,
      SummableGrid<int>* image_integrals) {
    for (uint32_t octant = 0; i < octant; ++octant) {
      image_integrals[i] = intersections;
      image_integrals[i].OrientedImageIntegral(OctantToOrientation(octant));
    }
  }

  void InitGrids(SummableGrid<int>* grids, int k) {
    for (int i = 0; i < k; ++k) {
      grids[i].Init();
      grids[i].AssignToAll(0);
    }
  }

  uint32_t OrientationToOctant(const glm::ivec3& orientation) const {
    glm::ivec3 bits = (1 - orientation) >> 1;
    uint32_t octant = bits[0] | (bits[1] << 1) | (bits[2] << 2);
    return octant;
  }

  glm::ivec3 OctantToOrientation(uint32_t octant) const {
    glm::ivec3 bits = glm::ivec3(octant & 0x1, (octant & 0x2) >> 1,
        (octant & 0x4) >> 2);
    glm::ivec3 orientation = (bits + 1) << 1;
    return orientation;
  }

  virtual void FindMinCostPoint(const ObjectVector& objects,
      const BoundingBox& bounds, const SummableGrid<int>& cell_intersections,
      int num_samples, float& min_cost, glm::ivec3& best_point) {
    glm::ivec3 size(num_samples, num_samples, num_samples);
    UniformGridSampler sampler(size, bounds);

    //  sample each counting function using image integral
    SummableGrid<int> image_integrals[8];
    InitGrids(&image_integrals[0], 8);
    ComputeImageIntegrals(cell_intersections, &image_integrals[0]);

    // find lowest cost vertex
    float best_cost = std::numeric_limits<float>::max();
    float area = 0.0f, count = 0.0f;
    glm::vec3 best = glm::vec3(0.0f);
    glm::ivec3 index = glm::ivec3(0);
    glm::ivec3 bits = glm::ivec3(0);
    BoundingBox octant_bounds = BoundingBox();
    for (int n = 0; n < num_samples * num_samples * num_samples; ++n) {
      glm::vec3 point = sampler.GetVertexAt(index);
      for (uint32_t octant = 0; octant < 8; ++octant) {
        bits = GetOctantBits(octant);
        octant_bounds = GetOctantBounds(point, bounds, octant);
        area = octant_bounds.GetArea();
        count = static_cast<float>(image_integrals[octant].GetSafe(index - bits,
            0));
        cost += area * count;
      }
      if (cost < best_cost) {
        best = point;
        best_cost = cost;
      }
      index = GridBase::Step(index, size);
    }
    best_point = point;
    min_cost = best_cost;
  }

  virtual float EvaluateCost(const ObjectVector& objects,
      const BoundingBox& bounds, int num_samples, glm::ivec3& split) {
    glm::ivec3 size(num_samples, num_samples, num_samples);
    SummableGrid<int> cell_intersections(size - 1);
    UniformGridSampler sampler(size, bounds);

    // compute intersections
    cell_intersections.Init();
    cell_intersections.AssignToAll(0);
    ComputeIntersections(objects, bounds, cell_intersections);

    //  sample each counting function using image integral
    SummableGrid<int> image_integrals[8];
    InitGrids(&image_integrals[0], 8);
    ComputeImageIntegrals(cell_intersections, &image_integrals[0]);

    // compute uncertainty across each cell
    SummableGrid<int> errors(size - 1);
    errors.Init();
    errors.AssignToAll(0);
    glm::ivec3 index = glm::ivec3(0);
    int i, j, k, A, B, C, D, sum = 0;
    int N0, N1, N2, N3, N4, N5, N6, N7, E;
    for (int n = 0; n < errors.grid_size(); ++n) {
      i = index[0];
      j = index[1];
      k = index[2];
      // for each vertex, compute the corresponding counting function
      N0 = image_integrals[0].GetSafe(i - 1, j - 1, k - 1, 0);
      N1 = image_integrals[1].GetSafe(i - 1, j, k, 0);
      N2 = image_integrals[2].GetSafe(i - 1, j, k, 0);
      N3 = image_integrals[3].GetSafe(i, j, k, 0);
      N4 = image_integrals[4].GetSafe(i + 1, j - 1, k - 1, 0);
      N5 = image_integrals[5].GetSafe(i + 1, j + 1, k, 0);
      N6 = image_integrals[6].GetSafe(i + 1, j, k, 0);
      N7 = image_integrals[7].GetSafe(i + 1, j + 1, k + 1, 0);
      // compute the absolute value of the difference between antipodal
      // vertices of the cell
      A = abs(N7 - N0);
      B = abs(N6 - N1);
      C = abs(N5 - N2);
      D = abs(N4 - N3);
      // take the product as the error
      E = A * B * C * D;
      errors(index) = E;
      sum += E;
      index = errors.Step(index);
    }

    // adaptively sample each cell, only using a total of
    // N = num_samples * num_samples * num_samples
    float ratio = 0.0f;
    int M = 0, N = num_samples * num_samples * num_samples;
    float best_cost = std::numeric_limits<float>::max();
    glm::vec3 best_point = glm::vec3(0.0f);
    for (int n = 0; n < errors.grid_size(); ++n) {
      // compute M = cube_root( (error(cell) / total_error) * N)
      ratio = static_cast<float>(errors(index)) / static_cast<float>(sum);
      M = static_cast<int>(roundf(pow(ratio, 1.0f / 3.0f)));
      // populate grid and compute image integral

      index = errors.Step(index);
    }
    return min_cost;
  }

  virtual void BuildInternal(SAHOctNode& node, WorkNodeType& work_node,
      WorkListType& next_list, uint32_t depth) {
    ++this->num_internal_nodes_;
    WorkNodeType child_work_nodes[8]; // process children tentatively
    node.set_offset(this->nodes_.size()); // children will have nodes pushed
    for (uint32_t j = 0; j < 8; ++j)
      child_work_nodes[j] = WorkNodeType( // initialize child lists
          this->GetChildBounds(node, work_node.bounds, j));
    while (!work_node.objects.empty()) {
      const SceneObject* obj = work_node.objects.back();
      work_node.objects.pop_back();
      for (uint32_t j = 0; j < 8; ++j)  // distribute to children
        if (obj->GetBounds().Overlap(child_work_nodes[j].bounds))
          child_work_nodes[j].objects.push_back(obj);
    }
    for (uint32_t j = 0; j < 8; ++j) {
      // If a child has a non-empty object list, process it.
      if (child_work_nodes[j].objects.size() > 0) {
        node.set_size(node.size() + 1); // update parent size
        uint32_t count = child_work_nodes[j].objects.size();
        glm::vec3 split = glm::vec3(0.0f);
        float cost = EvaluateCost(child_work_nodes[j].objects, 8,
            child_work_nodes[j].bounds, split);
        SAHOctNode child;
        if (cost <= this->GetMinCost() || depth + 1 >= this->GetMaxDepth()
            || count <= this->GetMaxLeafSize())
          child = this->GetNodeFactory().CreateLeaf(j);
        else
          child = this->GetNodeFactory().CreateInternal(j);
        child_work_nodes[j].node_index = this->nodes_.size();
        next_list.push_back(child_work_nodes[j]);
        this->nodes_.push_back(this->EncodeNode(child));
      }
    }
  }
};
}
#endif /* SAH_OCTREE_HPP_ */
