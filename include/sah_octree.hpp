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
#include "sah_octnode.hpp"
#include "octree.hpp"
#include "octree_base.hpp"
#include "shape.hpp"
namespace ray {
template<class SceneObject, int NumSamples>
class SAHOctree: public Octree<SceneObject, SAHOctNode, SAHEncodedNode,
    SAHOctNodeFactory, 1, std::numeric_limits<uint32_t>::max()> {
public:
  typedef std::vector<const SceneObject*> ObjectVector;

  SAHOctree() :
          Octree<SceneObject, SAHOctNode, SAHEncodedNode, SAHOctNodeFactory, 1,
              std::numeric_limits<uint32_t>::max()>::Octree(), nodes_(),
          scene_objects_(), bounds_(), num_internal_nodes_(0), num_leaves_(0) {
  }

  virtual ~SAHOctree() {
  }

  virtual uint32_t PointToOctant(const OctNode& node, const BoundingBox& bounds,
      const glm::vec3& point) const {
    glm::vec3 split = node.GetPoint();
    uint32_t x_bit = (point[0] > split[0]);
    uint32_t y_bit = (point[1] > split[1]);
    uint32_t z_bit = (point[2] > split[2]);
    uint32_t octant = x_bit | (y_bit << 1) | (z_bit << 2);
    return octant;
  }

  virtual BoundingBox GetChildBounds(const OctNode& node,
      const BoundingBox& bounds, uint32_t octant) const {
    glm::vec3 split = node.GetPoint();
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

  virtual float SampleCounts(const ObjectVector& objects,
      const BoundingBox& bounds, const glm::vec3 direction, float* counts) {
    uint32_t total_num_samples = NumSamples * NumSamples * NumSamples;
    BoundingBox* boxes;

    for (uint32_t i = 0; i < objects.size(); ++i) {
      for (uint32_t j = 0; j < total_num_samples; ++j) {

      }
    }
    return 0.0f;
  }

  virtual float EvaluateCost(WorkNode& work_node, glm::vec3& best_split) {
    best_split = work_node.bounds.GetCenter();
    return 0.0f;
  }

  virtual void BuildInternal(SAHOctNode& node, WorkNode& work_node,
      WorkList& next_list, uint32_t depth) {
    ++num_internal_nodes_;
    WorkNode child_work_nodes[8]; // process children tentatively
    node.set_offset(nodes_.size()); // children will have nodes pushed
    for (uint32_t j = 0; j < 8; ++j)
      child_work_nodes[j] = WorkNode( // initialize child lists
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
        OctNode child;
        if (cost <= min_cost || depth + 1 >= max_depth
            || count <= max_leaf_size)
          child = this->GetNodeFactory().CreateLeaf(j);
        else
          child = this->GetNodeFactory().CreateInternal(j);
        child_work_nodes[j].node_index = nodes_.size();
        next_list.push_back(child_work_nodes[j]);
        nodes_.push_back(EncodeNode(child));
      }
    }
  }
};
}
#endif /* SAH_OCTREE_HPP_ */
