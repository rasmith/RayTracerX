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
  typedef std::vector<const SceneObject*> ObjectVector;

  Kdtree() :
      TreeBase<SceneObject, Node, EncodedNode, NodeFactory>::TreeBase(),
          split_policy_(kSpatialMedian) {
  }

  virtual ~Kdtree() {
  }
protected:
  enum SplitResult {
    kLeaf = 0, kSplitX, kSplitY, kSplitZ
  };
  enum SplitPolicy {
    kSpatialMedian = 0, kFullSAH = 1
  };
  typedef TreeBase<SceneObject, Node, EncodedNode, NodeFactory> TreeType;
  typedef typename TreeType::WorkNode WorkNodeType;
  typedef typename TreeType::WorkList WorkListType;

  SplitPolicy split_policy_;

  virtual BoundingBox GetChildBounds(const Node& node,
      const BoundingBox& bounds, uint32_t i) const {
    BoundingBox child_bounds;
    return child_bounds;
  }

  virtual Node GetIthChildOf(const Node& node, uint32_t i) const {
    return nodes_[node.offset() + i];
  }

  void EvaluateSpatialMedian(Node& child, WorkNodeType& child_work,
      float& split_value, SplitResult& split_result) {
    static uint16_t current_dim = 0;

  }

  void EvaluateFullSAH(Node& child, WorkNodeType& child_work,
      float& split_value, SplitResult& split_result) {
  }

  void EvaluateSplit(Node& child, WorkNodeType& child_work, float& split_value,
      SplitResult& split_result) {
    switch (split_policy_) {
    case kSpatialMedian:
      EvaluateSpatialMedian(child, child_work_nodes[j], split_value,
          split_result);
      break;
    case kFullSAH:
      EvaluateFullSAH(child, child_work_nodes[j], split_value, split_result);
      break;
    default:
      EvaluateSpatialMedian(child, child_work_nodes[j], split_value,
          split_result);
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
    ++num_leaves_;
    node.set_offset(scene_objects_.size());
    node.set_size(work_node.objects.size());
    while (!work_node.objects.empty()) {
      scene_objects_.push_back(work_node.objects.back());
      work_node.objects.pop_back();
    }
  }

  virtual void BuildInternal(OctNode& node, WorkNodeType& work_node,
      WorkListType& next_list, uint32_t depth) {
    ++num_internal_nodes_;
    WorkNodeType child_work_nodes[2]; // process children tentatively
    node.set_offset(nodes_.size()); // children will have nodes pushed
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
        node.set_size(node.size() + 1); // update parent size
        uint32_t count = child_work_nodes[j].objects.size();
        Node child;
        float split_value;
        SplitResult split_result;
        EvaluateSplit(child, child_work_nodes[j], split_value, split_result);
        if (kLeaf == split_result)
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
} // namespace ray
#endif /* KDTREE_HPP_ */
