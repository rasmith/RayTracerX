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
template<class SceneObject, int max_leaf_size = 16, int max_depth = 10>
class SAHOctree: public Octree<SceneObject, SAHOctNode, SAHEncodedNode,
    SAHOctNodeFactory, max_leaf_size, max_depth> {
public:
  typedef std::vector<const SceneObject*> ObjectVector;

  SAHOctree() :
          Octree<SceneObject, SAHOctNode, SAHEncodedNode, SAHOctNodeFactory,
              max_leaf_size, max_depth>::Octree() {
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
        octant_bounds.min()[i] = point[i];
      else
        octant_bounds.max()[i] = point[i];
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

  float GetLeafCost(const ObjectVector& objs, const BoundingBox& bounds) const {
    return objs.size() * bounds.GetArea();
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
    glm::ivec3 orientation = 1 - (bits << 1);
    return orientation;
  }

  virtual void EvaluateCost(const ObjectVector& objects,
      const BoundingBox& bounds, float& cost, glm::ivec3& split) {
    int num_samples = 16;
    if (objects.size() < (2 << 20) && objects.size() >= (2 << 16))
      num_samples = 8;
    if (objects.size() < (2 << 16) && objects.size() >= (2 << 8))
      num_samples = 4;
    glm::ivec3 size(num_samples, num_samples, num_samples);
    SummableGrid<int> cell_intersections(size - 1);
    UniformGridSampler sampler(size, bounds);

    glm::ivec3 index = glm::ivec3(0);
    SceneObject* obj = NULL;
    BoundingBox obj_bounds = BoundingBox();
    glm::ivec3 bits = glm::ivec3(0);
    glm::vec3 point = glm::vec3(0.0f);

    // populate image integrals
    SummableGrid<int> image_integrals[8];
    for (int i = 0; i < objects.size(); ++i) {
      obj = objects[i];
      obj_bounds = obj->GetBounds();
      for (int octant = 0; octant < 8; ++octant) {
        bits = GetOctantBits(octant);
        for (int d = 0; d < 3; ++d)
          point[d] = (bits[d] == 0 ? obj_bounds.min()[d] : obj_bounds.max()[d]);
        if (sampler.PointToCellIndex(point, index))
          ++image_integrals[octant](index);
      }
    }

    //  sample each counting function using image integral
    InitGrids(&image_integrals[0], 8);
    for (uint32_t octant = 0; octant < 8; ++octant) {
      image_integrals[octant].OrientedImageIntegral(
          OctantToOrientation(octant));
    }

    // find lowest cost vertex
    float best_cost = std::numeric_limits<float>::max();
    float area = 0.0f, count = 0.0f;
    glm::vec3 best_point = glm::vec3(0.0f);
    BoundingBox octant_bounds = BoundingBox();
    index = glm::ivec3(0);
    for (int n = 0; n < size[0] * size[1] * size[2]; ++n) {
      point = sampler.GetVertexAt(index);
      for (uint32_t octant = 0; octant < 8; ++octant) {
        bits = GetOctantBits(octant);
        octant_bounds = GetOctantBounds(point, bounds, octant);
        area = octant_bounds.GetArea();
        count = static_cast<float>(image_integrals[octant].GetSafe(index - bits,
            0));
        cost += area * count;
      }
      if (cost < best_cost) {
        best_point = point;
        best_cost = cost;
      }
      index = GridBase::Step(index, size);
    }
    split = best_point;
    cost = best_cost;
  }

  virtual void BuildRoot(SAHOctNode& root, WorkNodeType& work_root) {
    glm::vec3 split = glm::vec3(0.0f);
    float cost = 0.0f;
    EvaluateCost(work_root.objects, work_root.bounds, cost, split);
    if (cost > this->GetLeafCost(work_root.objects, work_root.bounds)
        || (0 == max_depth) || (work_root.objects.size() <= max_leaf_size))
      root = this->GetNodeFactory().CreateLeaf(0);
    else {
      root = this->GetNodeFactory().CreateInternal(0);
      root.set_point(split);
    }
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
        float cost = 0.0f;
        EvaluateCost(child_work_nodes[j].objects, child_work_nodes[j].bounds,
            cost, split);
        SAHOctNode child;
        if (cost
            > this->GetLeafCost(child_work_nodes[j].objects,
                child_work_nodes[j].bounds) || depth + 1 >= this->GetMaxDepth()
            || count <= this->GetMaxLeafSize())
          child = this->GetNodeFactory().CreateLeaf(j);
        else {
          child = this->GetNodeFactory().CreateInternal(j);
          child.set_point(split);
        }
        child_work_nodes[j].node_index = this->nodes_.size();
        next_list.push_back(child_work_nodes[j]);
        this->nodes_.push_back(this->EncodeNode(child));
      }
    }
  }
};
}
#endif /* SAH_OCTREE_HPP_ */
