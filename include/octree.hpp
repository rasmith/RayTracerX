/*
 * octree.hpp
 *
 *  Created on: Dec 19, 2013
 *      Author: agrippa
 */

#ifndef OCTREE_HPP_
#define OCTREE_HPP_
#include <sys/types.h>
#include <stdint.h>
#include <algorithm>
#include <cstring>
#include <list>
#include <vector>
#include "octree_base.hpp"
#include "shape.hpp"
namespace ray {
template<class SceneObject>
class Octree: public OctreeBase {
public:
  typedef std::vector<const SceneObject*> ObjectVector;

  Octree() :
      OctreeBase(), nodes_(), scene_objects_(), bounds_(),
          num_internal_nodes_(0), num_leaves_(0) {
  }

  virtual ~Octree() {
    nodes_.clear();
    scene_objects_.clear();
  }

  virtual BoundingBox GetBounds() const {
    return bounds_;
  }

  void Build(const ObjectVector& objects) {
    bounds_ = BoundingBox();
    scene_objects_.clear();
    nodes_.clear();
    BuildTree(objects);
  }

  void Build(const std::vector<SceneObject>& objects) {
    bounds_ = BoundingBox();
    scene_objects_.clear();
    nodes_.clear();
    BuildTree(objects);
  }

private:
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

  std::vector<EncodedNode> nodes_;
  ObjectVector scene_objects_;
  BoundingBox bounds_;
  uint32_t num_internal_nodes_;
  uint32_t num_leaves_;

  OctNode DecodeNode(const EncodedNode& encoded) const {
    return GetNodeFactory().CreateOctNode(encoded);
  }

  EncodedNode EncodeNode(const OctNode& node) const {
    return GetNodeFactory().CreateEncodedNode(node);
  }

  virtual OctNode GetIthChildOf(const OctNode& node, uint32_t index) const {
    return DecodeNode(nodes_[node.offset() + index]);
  }

  virtual OctNode GetRoot() const {
    return DecodeNode(nodes_[0]);
  }

  uint32_t num_internal_nodes() {
    return num_internal_nodes_;
  }

  uint32_t num_leaves() {
    return num_leaves_;
  }

  virtual bool IntersectLeaf(const OctNode& leaf, const Ray& ray,
      Isect& isect) const {
    //std::cout << "IntersectLeaf: leaf = " << leaf << "\n";
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

  void BuildLeaf(OctNode& node, WorkNode& work_node) {
    ++num_leaves_;
    node.set_offset(scene_objects_.size());
    node.set_size(work_node.objects.size());
    while (!work_node.objects.empty()) {
      scene_objects_.push_back(work_node.objects.back());
      work_node.objects.pop_back();
    }
  }

  void BuildInternal(OctNode& node, WorkNode& work_node, WorkList& next_list,
      uint32_t depth) {
    ++num_internal_nodes_;
    WorkNode child_work_nodes[8]; // process children tentatively
    node.set_offset(nodes_.size()); // children will have nodes pushed
    for (uint32_t j = 0; j < 8; ++j)
      child_work_nodes[j] = WorkNode( // initialize child lists
          GetChildBounds(work_node.bounds, j));
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
        OctNode child;
        if (depth + 1 >= kMaxDepth || count <= kMaxLeafSize)
          child = GetNodeFactory().CreateLeaf(j);
        else
          child = GetNodeFactory().CreateInternal(j);
        child_work_nodes[j].node_index = nodes_.size();
        next_list.push_back(child_work_nodes[j]);
        nodes_.push_back(EncodeNode(child));
      }
    }
  }

  void BuildLevel(WorkList& work_list, WorkList& next_list, uint32_t depth) {
    while (!work_list.empty()) {
      WorkNode work_node = work_list.back();
      work_list.pop_back();
      OctNode node = DecodeNode(nodes_[work_node.node_index]);
      if (node.IsLeaf())
        BuildLeaf(node, work_node);
      else
        BuildInternal(node, work_node, next_list, depth);
      nodes_[work_node.node_index] = EncodeNode(node);
    }
  }

  void BuildTree(WorkNode& work_root) {
    // compute bounds
    for (uint32_t i = 0; i < work_root.objects.size(); ++i)
      bounds_ = bounds_.Join(work_root.objects[i]->GetBounds());
    std::vector<WorkNode> work_list;
    std::vector<WorkNode> next_list;
    int depth = 0;
    OctNode root; // Create and insert root
    if ((0 == OctreeBase::kMaxDepth)
        || (work_root.objects.size() <= kMaxLeafSize))
      root = GetNodeFactory().CreateLeaf(0);
    else
      root = GetNodeFactory().CreateInternal(0);
    nodes_.push_back(EncodeNode(root));
    work_root.bounds = bounds_;
    work_root.node_index = 0;
    work_list.push_back(work_root);
    next_list.clear();
    // This is a breadth first build. There are two work lists: work_list
    // and next_list.  The work_list gets swapped when empty while next_list
    // fills up. Each time this happens, one level has been completed.
    while (!work_list.empty()) {
      BuildLevel(work_list, next_list, depth);
      work_list.swap(next_list);
      std::cout << "level = " << depth << std::endl;
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
}
#endif /* OCTREE_HPP_ */
