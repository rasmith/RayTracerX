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
    BoundingBox current_bounds;
    UniformGridSampler sampler(intersections.size(), bounds);
    for (int i = 0; i < objects.size(); ++i) {
      for (int n = 0; n < intersections.grid_size(); ++n) {
        current_bounds = sampler.GetBoundsAt(index);
        index = intersections.Step(index);
        ++isects(index);
      }
    }
  }

  void ComputeImageIntegrals(const SummableGrid<int>& intersections,
      SummableGrid<int>* image_integrals, glm::ivec3* orientations) {
    for (int i = 0; i < 8; ++i) {
      image_integrals[i] = intersections;
      image_integrals[i].OrientedImageIntegral(orientations[i]);
    }
  }

  void InitGrids(SummableGrid<int>* grids, int k) {
    for (int i = 0; i < k; ++k) {
      grids[i].Init();
      grids[i].AssignToAll(0);
    }
  }

  virtual void FindMinCostPoint(const ObjectVector& objects,
      const BoundingBox& bounds, int num_samples, float& min_cost,
      glm::ivec3& best_point) {
    glm::ivec3 size(num_samples, num_samples, num_samples);
    SummableGrid<int> cell_intersections(size - 1);
    UniformGridSampler sampler(size, bounds);
    glm::ivec3 orientations[8] = { glm::ivec3(1, 1, 1), glm::ivec3(1, 1, -1),
        glm::ivec3(1, -1, 1), glm::ivec3(1, -1, -1), glm::ivec3(-1, 1, 1),
        glm::ivec3(-1, 1, -1), glm::ivec3(-1, -1, 1), glm::ivec3(-1, -1, -1) };

    // compute intersections
    cell_intersections.Init();
    cell_intersections.AssignToAll(0);
    ComputeIntersections(objects, bounds, cell_intersections);

    //  sample each counting function using image integral
    SummableGrid<int> image_integrals[8];
    InitGrids(&image_integrals[0], 8);
    ComputeImageIntegrals(cell_intersections, &image_integrals[0],
        &orientations[0]);

    // find lowest cost vertex
    int N[8];
    glm::ivec3 index = glm::ivec3(0);
    for (int n = 0; n < cell_intersections.grid_size(); ++n) {
      for (int i = 0; i < 8; ++i) {
        N[i] = image_integrals[i].GetSafe(index - ((orientations[i] + 1) >> 1),
            0);
      }
    }

  }

  virtual float EvaluateCost(const ObjectVector& objects,
      const BoundingBox& bounds, int num_samples, glm::ivec3& split) {
    glm::ivec3 size(num_samples, num_samples, num_samples);
    SummableGrid<int> cell_intersections(size - 1);
    UniformGridSampler sampler(size, bounds);
    glm::ivec3 orientations[8] = { glm::ivec3(1, 1, 1), glm::ivec3(1, 1, -1),
        glm::ivec3(1, -1, 1), glm::ivec3(1, -1, -1), glm::ivec3(-1, 1, 1),
        glm::ivec3(-1, 1, -1), glm::ivec3(-1, -1, 1), glm::ivec3(-1, -1, -1) };

    // compute intersections
    cell_intersections.Init();
    cell_intersections.AssignToAll(0);
    ComputeIntersections(objects, bounds, cell_intersections);

    //  sample each counting function using image integral
    SummableGrid<int> image_integrals[8];
    InitGrids(&image_integrals[0], 8);
    ComputeImageIntegrals(cell_intersections, &image_integrals[0],
        &orientations[0]);

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
      N0 = image_integrals[0].GetSafe(i - 1, j - 1, k - 1, 0);
      N1 = image_integrals[1].GetSafe(i - 1, j, k, 0);
      N2 = image_integrals[2].GetSafe(i - 1, j, k, 0);
      N3 = image_integrals[3].GetSafe(i, j, k, 0);
      N4 = image_integrals[4].GetSafe(i + 1, j - 1, k - 1, 0);
      N5 = image_integrals[5].GetSafe(i + 1, j + 1, k, 0);
      N6 = image_integrals[6].GetSafe(i + 1, j, k, 0);
      N7 = image_integrals[7].GetSafe(i + 1, j + 1, k + 1, 0);
      A = abs(N7 - N0);
      B = abs(N6 - N1);
      C = abs(N5 - N2);
      D = abs(N4 - N3);
      E = A * B * C * D;
      errors(i, j, k) = E;
      sum += E;
      index = errors.Step(index);
    }

    for (int n = 0; n < errors.grid_size(); ++n) {
      FindMinPoint(objects, sampler.GetBoundsAt(index),
          static_cast<float>(errors(index))
              / static_cast<float>((num_samples * num_samples * num_samples)),
          min_cost, split);
      index = errors.Step(index);
    }
    return 0.0f;
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
