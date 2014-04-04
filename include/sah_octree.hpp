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

  virtual float EvaluateCost(WorkNodeType& work_node, glm::vec3&) {
    glm::ivec3 size(8, 8, 8);
    Grid<int> vertex_costs(size);
    Grid<int> cell_counts(size - 1);
    Grid<BoundingBox> cell_bounds(size - 1);
    UniformGridSampler s(size, work_node.bounds);
    cell_counts.Init();
    cell_bounds.Init();
    glm::ivec3 index(0);
    for (int n = 0; n < cell_counts.grid_size(); ++n) {
      cell_bounds(index) = s.GetBoundsAt(index);
      index = cell_counts.Step(index);
    }
    for (int i = 0; i < work_node.objects.size(); ++i) {
      index = glm::ivec3(0);
      for (int n = 0; n < cell_counts.grid_size(); ++n) {
        if (cell_bounds[n].Overlap(work_node.objects.GetBounds()))
          ++cell_counts[n];
      }
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
        float cost = EvaluateCost(child_work_nodes[j], split);
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
