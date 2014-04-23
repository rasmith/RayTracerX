/*
 * tree_base.hpp
 *
 *  Created on: Apr 22, 2014
 *      Author: agrippa
 */

#ifndef TREE_BASE_HPP_
#define TREE_BASE_HPP_
#include <stdint.h>
#include <sys/types.h>
#include "scene.hpp"
#include "shape.hpp"
namespace ray {
template<class SceneObject, class Node, class EncodedNode, class NodeFactory>
class TreeBase: public Accelerator {
public:
  typedef std::vector<const SceneObject*> ObjectVector;

  TreeBase() :
      Accelerator(), max_leaf_size_(0), max_depth_(0), num_internal_nodes_(0),
          num_leaves_(0), nodes_(), scene_objects_(), bounds_() {
  }

  virtual ~TreeBase() {
    nodes_.clear();
    scene_objects_.clear();
  }

  virtual bool Intersect(const Ray& ray, Isect& isect) const {
    return Traverse(GetRoot(), bounds_, ray, isect, 0);
  }

  virtual void Print(std::ostream& out) const {
    std::vector<Node> nodes;
    std::vector<int> depths;
    std::vector<BoundingBox> bounds;
    nodes.clear();
    depths.clear();
    bounds.clear();
    nodes.push_back(GetRoot());
    bounds.push_back(bounds_);
    depths.push_back(0);
    while (!nodes.empty()) {
      Node node = nodes.back();
      int depth = depths.back();
      BoundingBox bbox = bounds.back();
      nodes.pop_back();
      bounds.pop_back();
      depths.pop_back();
      Node(out, node, bbox, depth);
      if (node.IsInternal()) {
        for (uint32_t i = 0; i < node.size(); ++i) {
          Node child = GetIthChildOf(node, i);
          nodes.push_back(child);
          bounds.push_back(GetChildBounds(node, bbox, child.index()));
          depths.push_back(depth + 1);
        }
      }
    }
  }

  uint32_t max_leaf_size() const {
    return max_leaf_size_;
  }

  uint32_t max_depth() const {
    return max_depth_;
  }
protected:
  struct WorkNode {
    WorkNode() :
        node_index(0), bounds(), objects() {
    }
    WorkNode(const BoundingBox& bbox) :
        node_index(0), bounds(bbox), objects() {
      objects.clear();
    }
    uint32_t node_index;
    BoundingBox bounds;
    ObjectVector objects;
  };
  typedef std::vector<WorkNode> WorkList;

  uint32_t max_leaf_size_;
  uint32_t max_depth_;
  uint32_t num_internal_nodes_;
  uint32_t num_leaves_;
  std::vector<EncodedNode> nodes_;
  ObjectVector scene_objects_;
  BoundingBox bounds_;

  virtual BoundingBox GetChildBounds(const Node& node,
      const BoundingBox& bounds, uint32_t index) const = 0;
  virtual Node GetIthChildOf(const Node& node, uint32_t index) const = 0;

  virtual void BuildRoot(Node& root, WorkNode& work_root) = 0;
  virtual void BuildLeaf(Node& node, WorkNode& work_node) = 0;
  virtual void BuildInternal(Node& node, Node& work_node, WorkList& next_list,
      uint32_t depth) = 0;

  virtual void BuildInternal(Node& node, WorkNode& work_node,
      WorkList& next_list, uint32_t depth) = 0;

  virtual Node GetRoot() const {
    return nodes_[0];
  }

  virtual BoundingBox bounds() const {
    return bounds_;
  }

  Node DecodeNode(const EncodedNode& encoded) const {
    return this->GetNodeFactory().CreateOctNode(encoded);
  }

  EncodedNode EncodeNode(const Node& node) const {
    return this->GetNodeFactory().CreateEncodedNode(node);
  }

  uint32_t num_internal_nodes() {
    return num_internal_nodes_;
  }

  uint32_t num_leaves() {
    return num_leaves_;
  }

  virtual bool IntersectLeaf(const Node& leaf, const Ray& ray,
      Isect& isect) const {
    bool hit = false;
    Isect current;
    Isect best;
    best.t_hit = std::numeric_limits<float>::max();
    const SceneObject* const * objects = &scene_objects_[leaf.offset()];
    for (uint32_t i = 0; i < leaf.size(); ++i)
      if (objects[i]->Intersect(ray, current) && current.t_hit < best.t_hit) {
        best = current;
        hit = true;
      }
    if (hit)
      isect = best;
    return hit;
  }

  bool Traverse(const Node& node, const BoundingBox& bounds, const Ray& ray,
      Isect& isect, uint32_t depth) const {
    if (depth > max_depth_) // check depth first
      return false;
    float t_near, t_far;
    if (!bounds.Intersect(ray, t_near, t_far)) // check bounds next
      return false;
    if (node.IsLeaf()) // is this a leaf?
      return IntersectLeaf(node, ray, isect);
    Node children[node.size()];
    BoundingBox child_bounds[node.size()];
    uint32_t count = 0;
    IntersectChildren(node, bounds, ray, &children[0], &child_bounds[0], count);
    bool hit = false;
    for (uint32_t i = 0; i < count && !hit; ++i)
      hit = Traverse(children[i], child_bounds[i], ray, isect, depth + 1);
    return hit;
  }

  virtual NodeFactory& GetNodeFactory() const {
    return NodeFactory::GetInstance();
  }

///////
//
// IntersectChildren
//
//  Intersect children of given node and return in sorted order
//  that the given ray hit them along with child bounding boxes.
//
//////
  virtual void IntersectChildren(const Node& node, const BoundingBox& bounds,
      const Ray& ray, Node* children, BoundingBox* child_bounds,
      uint32_t& count) const = 0;

  virtual void PrintNode(std::ostream& out, const Node& node,
      const BoundingBox& bbox, int depth) const {
    for (int i = 0; i < depth; ++i)
      out << " ";
    out << node << " " << bbox << "\n";
  }

  void UpdateMeanVar(int value, int num_samples, float& mean, float& variance) {
    float delta = value - mean;
    mean += delta / num_samples;
    variance += delta * (value - mean);
  }

  void BuildLevel(WorkList& work_list, WorkList& next_list, uint32_t depth) {
    float mean_objects = 0.0f, variance_objects = 0.0f;
    float mean_children = 0.0f, variance_children = 0.0f;
    int num_nodes = 0, num_internal = 0, num_leaves = 0;
    while (!work_list.empty()) {
      WorkNode work_node = work_list.back();
      work_list.pop_back();
      // calculate some stats
      UpdateMeanVar(work_node.objects.size(), ++num_nodes, mean_objects,
          variance_objects);
      Node node = DecodeNode(nodes_[work_node.node_index]);
      if (node.IsLeaf()) {
        BuildLeaf(node, work_node);
        ++num_leaves;
      } else {
        BuildInternal(node, work_node, next_list, depth);
        UpdateMeanVar(node.size(), ++num_internal, mean_children,
            variance_children);
      }
      nodes_[work_node.node_index] = EncodeNode(node);
    }
    variance_objects /= num_nodes;
    variance_children /= num_internal;
    std::cout << " internal: " << num_internal << " num_leaves = "
        << num_leaves;
    std::cout << " mean obj: " << round(mean_objects) << " stdev: "
        << round(100.0f * sqrt(variance_objects)) / 100.0f;
    std::cout << " mean child: " << round(mean_children) << " stdev: "
        << round(100.0f * sqrt(variance_children)) / 100.0f;
  }

  void BuildTree(WorkNode& work_root) {
    // compute bounds
    for (uint32_t i = 0; i < work_root.objects.size(); ++i)
      bounds_ = bounds_.Join(work_root.objects[i]->GetBounds());
    std::vector<WorkNode> work_list;
    std::vector<WorkNode> next_list;
    int depth = 0;
    Node root; // Create and insert root
    work_root.bounds = bounds_;
    work_root.node_index = 0;
    BuildRoot(root, work_root);
    nodes_.push_back(EncodeNode(root));
    work_list.push_back(work_root);
    next_list.clear();
    // This is a breadth first build. There are two work lists: work_list
    // and next_list.  The work_list gets swapped when empty while next_list
    // fills up. Each time this happens, one level has been completed.
    while (!work_list.empty()) {
      std::cout << "level = " << depth << " ";
      BuildLevel(work_list, next_list, depth);
      work_list.swap(next_list);
      std::cout << std::endl;
      ++depth;
    }
    std::cout << "num internal nodes = " << num_internal_nodes() << std::endl;
    std::cout << "num leaves = " << num_leaves() << std::endl;
    std::cout << "num object refs = " << scene_objects_.size() << std::endl;
  }

  void BuildTree(const std::vector<SceneObject>& objects) {
    WorkNode work_root = WorkNode(bounds_);
    for (uint32_t i = 0; i < objects.size(); ++i)
      work_root.objects.push_back(&objects[i]);
    BuildTree(work_root);
  }

  void BuildTree(const ObjectVector& objects) {
    WorkNode work_root = WorkNode(bounds_);
    for (uint32_t i = 0; i < objects.size(); ++i)
      work_root.objects.push_back(objects[i]);
    BuildTree(work_root);
  }
};
} // namespace ray
#endif /* TREE_BASE_HPP_ */