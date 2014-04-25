/*
 * kdtree.hpp
 *
 *  Created on: Apr 22, 2014
 *      Author: agrippa
 */

#ifndef KDTREE_HPP_
#define KDTREE_HPP_
#include <stdint.h>
#include <sys/types.h>
#include "scene.hpp"
#include "shape.hpp"
#include "tree_base.hpp"
namespace ray {
template<class SceneObject, class Node, class EncodedNode, class NodeFactory>
class Kdtree: public TreeBase<SceneObject, Node, EncodedNode, NodeFactory> {
public:
  enum SplitPolicy {
    kSpatialMedian = 0, kFullSAH = 1
  };
  typedef std::vector<const SceneObject*> ObjectVector;

  Kdtree() :
      TreeBase<SceneObject, Node, EncodedNode, NodeFactory>::TreeBase(),
          split_policy_(kSpatialMedian) {
  }

  virtual ~Kdtree() {
  }

  const SplitPolicy& split_policy() const {
    return split_policy_;
  }

  void set_split_policy(const SplitPolicy& policy) {
    split_policy_ = policy;
  }
protected:
  enum SplitResult {
    kSplitX = 0, kSplitY, kSplitZ, kLeaf
  };
  typedef TreeBase<SceneObject, Node, EncodedNode, NodeFactory> TreeType;
  typedef typename TreeType::WorkNode WorkNodeType;
  typedef typename TreeType::WorkList WorkListType;

  SplitPolicy split_policy_;

  virtual BoundingBox GetChildBounds(const Node& node,
      const BoundingBox& bounds, uint32_t i) const {
    BoundingBox child_bounds = bounds;
    uint32_t dim = static_cast<uint32_t>(node.type());
    if (i == 0)
      child_bounds.max()[dim] = node.split_value();
    else
      child_bounds.min()[dim] = node.split_value();
    return child_bounds;
  }

  void EvaluateSpatialMedian(Node& child, WorkNodeType& child_work,
      float& split_value, SplitResult& split_result) {
    uint32_t dim = static_cast<uint32_t>(child.type());
    dim = (dim + 1) % 3;
    split_value = child_work.bounds.GetCenter()[dim];
    split_result = static_cast<SplitResult>(dim);
  }

  void EvaluateFullSAH(Node&, WorkNodeType&, float&, SplitResult&) {
  }

  void EvaluateSplit(Node& child, WorkNodeType& child_work, float& split_value,
      SplitResult& split_result) {
    switch (split_policy_) {
    case kSpatialMedian:
      EvaluateSpatialMedian(child, child_work, split_value, split_result);
      break;
    case kFullSAH:
      EvaluateFullSAH(child, child_work, split_value, split_result);
      break;
    default:
      EvaluateSpatialMedian(child, child_work, split_value, split_result);
    }
  }

  virtual void IntersectChildren(const Node& node, const BoundingBox& bounds,
      const Ray& ray, float t_near, float t_far, Node* children,
      BoundingBox* child_bounds, uint32_t& count) const {
    uint32_t dim = static_cast<uint32_t>(node.type());
    const glm::vec3& origin = ray.origin();
    const glm::vec3& direction = ray.direction();
    float split_value = node.split_value();
    float t_split = (split_value - origin[dim]) / direction[dim];
    bool has_left, has_right;
    Node left, right, temp_node;
    BoundingBox left_bounds, right_bounds, temp_bounds;
    count = 0;
    for (uint32_t i = 0; i < node.num_children(); ++i) {
      temp_node = this->GetIthChildOf(node, i);
      temp_bounds = this->GetChildBounds(node, bounds, temp_node.order());
      if (0 == temp_node.order()) {
        has_left = true;
        left = temp_node;
        left_bounds = temp_bounds;
      } else {
        has_right = true;
        right = temp_node;
        right_bounds = temp_bounds;
      }
    }
    if (has_left && t_far < t_split) {
      count = 1;
      children[0] = left;
      child_bounds[0] = left_bounds;
    } else if (has_right && t_split > t_near) {
      count = 1;
      children[0] = right;
      child_bounds[0] = right_bounds;
    } else {
      int k = 0;
      if (has_left) {
        ++count;
        children[k] = left;
        child_bounds[k] = left_bounds;
        ++k;
      }
      if (has_right) {
        ++count;
        children[k] = right;
        child_bounds[k] = right_bounds;
      }
    }
  }

  virtual void BuildRoot(Node& root, WorkNodeType& work_root) {
    float split_value;
    SplitResult split_result;
    EvaluateSplit(root, work_root, split_value, split_result);
    if (kLeaf == split_result)
      root = this->GetNodeFactory().CreateLeaf(0);
    else
      root = this->GetNodeFactory().CreateInternal(0);
  }

  virtual void BuildLeaf(Node& node, WorkNodeType& work_node) {
    ++this->num_leaves_;
    node.set_offset(this->scene_objects_.size());
    node.set_num_objects(work_node.objects.size());
    while (!work_node.objects.empty()) {
      this->scene_objects_.push_back(work_node.objects.back());
      work_node.objects.pop_back();
    }
  }

  virtual void BuildInternal(Node& node, WorkNodeType& work_node,
      WorkListType& next_list, uint32_t depth) {
    ++this->num_internal_nodes_;
    WorkNodeType child_work_nodes[2]; // process children tentatively
    node.set_offset(this->nodes_.size()); // children will have nodes pushed
    for (uint32_t j = 0; j < 2; ++j)
      child_work_nodes[j] = WorkNodeType( // initialize child lists
          this->GetChildBounds(node, work_node.bounds, j));
    while (!work_node.objects.empty()) {
      const SceneObject* obj = work_node.objects.back();
      work_node.objects.pop_back();
      for (uint32_t j = 0; j < 2; ++j)  // distribute to children
        if (obj->GetBounds().Overlap(child_work_nodes[j].bounds))
          child_work_nodes[j].objects.push_back(obj);
    }
    for (uint32_t j = 0; j < 2; ++j) {
      // If a child has a non-empty object list, process it.
      if (child_work_nodes[j].objects.size() > 0) {
        node.set_num_children(node.num_children() + 1); // update parent size
        uint32_t count = child_work_nodes[j].objects.size();
        Node child;
        float split_value;
        SplitResult split_result;
        EvaluateSplit(child, child_work_nodes[j], split_value, split_result);
        if (count <= this->max_leaf_size_ || depth >= this->max_depth_
            || kLeaf == split_result)
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
} // namespace ray
#endif /* KDTREE_HPP_ */
