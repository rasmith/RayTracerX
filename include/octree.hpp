/*
 * octree.hpp
 *
 *  Created on: Dec 19, 2013
 *      Author: agrippa
 */

#ifndef OCTREE_HPP_
#define OCTREE_HPP_
#include <stdint.h>
#include <algorithm>
#include <cstring>
#include <list>
#include <vector>
#include "octree_base.hpp"
#include "shape.hpp"
namespace ray {
template<class SceneObject>
class OctTree: public OctTreeBase {
public:
  virtual ~OctTree();
  virtual bool Intersect(const Ray& ray, Isect& isect) const {
    return Traverse(DecodeNode(nodes_[0]), bounds_, ray, isect, 0);
  }
  virtual BoundingBox GetBounds() {
    return bounds_;
  }
  virtual void Print(std::ostream& out) const;
  typedef std::vector<SceneObject*> ObjectVector;
  void Build(const ObjectVector& objects) {
    bounds_ = BoundingBox();
    scene_objects_.clear();
    nodes_.clear();
    for (uint32_t i = 0; i < objects.size(); ++i)
      bounds_ = bounds_.Join(objects[i]->GetBounds());
    BuildTree(objects);
  }
private:
  std::vector<EncodedNode> nodes_;
  ObjectVector scene_objects_;
  BoundingBox bounds_;
  OctNode DecodeNode(const EncodedNode& encoded) const {
    return GetNodeFactory().CreateOctNode(encoded);
  }
  EncodedNode EncodeNode(const OctNode& node) const {
    return GetNodeFactory().CreateEncodedNode(node);
  }
  bool IntersectLeaf(const OctNode& leaf, const BoundingBox& bounds,
      const Ray& ray, Isect& isect) const {
    bool hit = false;
    Isect current;
    Isect best;
    best.t_hit = std::numeric_limits<float>::max();
    SceneObject* const objects = scene_objects_[leaf.size_];
    for (uint32_t i = 0; i < leaf.size_(); ++i)
      if (objects[i]->Intersect(ray, current) && current.t_hit < best.t_hit) {
        best = current;
        hit = true;
      }
    if (hit) {
      isect = best;
    }
    return hit;
  }
  bool Traverse(const OctNode& node, const BoundingBox& bounds, const Ray& ray,
      Isect& isect, int depth) const {
    if (depth > kMaxDepth) // check depth first
      return false;
    float t_near, t_far;
    if (!bounds.Intersect(ray, t_near, t_far)) // check bounds next
      return false;
    if (node.IsLeaf()) // is this a leaf?
      return IntersectLeaf(node, bounds, ray, isect);
    // This is an internal node:  Decode each child and
    // do AABB intersection and sort the hit children by
    // t_near times.  Return first child that records an
    // object intersection.
    OctNode candidates[4]; // can hit at most four children
    BoundingBox child_bounds[4];
    float t_near_vals[4];
    float t_far_vals[4];
    int count = 0;
    EncodedNode* const children = &nodes_[node.offset_];
    for (int i = 0; count < 4 && i < node.size_; ++i) {
      candidates[count] = DecodeNode(children[i]);
      child_bounds[count] = GetChildBounds(bounds, candidates[count].octant_);
      if (child_bounds[count].Intersect(ray, t_near_vals[count],
          t_far_vals[count])) ++count;
    }
    // sort by t_near - selection sort
    for (int i = 0; i < count; ++i) {
      int *pos = std::min_element(t_near_vals + i, t_near_vals + count);
      int k = pos - &count[0];
      std::swap(candidates[i], candidates[k]);
      std::swap(child_bounds[i], child_bounds[k]);
      std::swap(t_near_vals[i], t_far_vals[k]);
      std::swap(t_far_vals[i], t_far_vals[k]);
    }
    bool hit = false;
    for (int i = 0; i < count && !hit; ++i)
      hit = Traverse(candidates[i], child_bounds[i], ray, isect, depth + 1);
    return hit;
  }
  struct WorkNode {
    WorkNode(const BoundingBox& bbox) :
        node_index(0), bounds(bbox), objects() {
      objects.clear();
    }
    uint32_t node_index;
    BoundingBox bounds;
    ObjectVector objects;
  };
  typedef std::vector<WorkNode> WorkList;
  void BuildLeaf(OctNode& node, WorkNode& work_node, WorkList& next_list,
      int depth) {
    node.offset_ = scene_objects_.size();
    node.size_ = work_node.objects.size();
    while (!work_node.objects.empty()) {
      scene_objects_.push_back(work_node.objects.back());
      work_node.objects.pop_back();
    }
  }
  void BuildInternal(OctNode& node, WorkNode& work_node, WorkList& next_list,
      int depth) {
    WorkNode child_work_nodes[8]; // process children tentatively
    node.offset_ = nodes_.size(); // children will have nodes pushed
    for (uint32_t j = 0; j < 8; ++j)
      child_work_nodes[j] = WorkNode( // initialize child lists
          GetChildBounds(work_node.bounds, j));
    while (!work_node.objects.empty()) {
      SceneObject* obj = work_node.objects.back();
      work_node.objects.pop_back();
      for (uint32_t j = 0; j < 8; ++j)  // distribute to children
        if (obj->GetBounds().Overlap(child_work_nodes[j].bounds))
          child_work_nodes[j].objects.push_back(obj);
    }
    for (uint32_t j = 0; j < 8; ++j) {
      // If a child has a non-empty object list, process it.
      if (child_work_nodes[j].object_list.size() > 0) {
        ++node.size_; // update parent size
        uint32_t count = child_work_nodes[j].objects.size();
        OctNode child;
        if (depth > kMaxDepth || count <= kMaxLeafSize)
          child = GetNodeFactory().CreateLeaf(j);
        else
          child = GetNodeFactory().CreateInternal(j);
        child_work_nodes[j].node_index = nodes_.size();
        next_list.push_back(child_work_nodes[j]);
        nodes_.push_back(EncodeNode(child));
      }
    }
  }
  void BuildLevel(WorkList& work_list, WorkList& next_list, int depth) {
    while (!work_list.empty()) {
      WorkNode work_node = work_list.back();
      work_list.pop_back();
      OctNode node = DecodeNode(nodes_[work_node.node_index]);
      if (node.IsLeaf())
        BuildLeaf(node, work_node, next_list, depth);
      else
        BuildInternal(node, work_node, next_list, depth);
      nodes_[work_node.node_index] = EncodeNode(node);
    }
  }
  void BuildTree(const ObjectVector& objects) {
    std::vector<WorkNode> work_list;
    std::vector<WorkNode> next_list;
    int depth = 0;
    // Create and insert root
    OctNode root = GetNodeFactory().CreateRoot();
    nodes_.push_back(EncodeNode(root));
    WorkNode work_root = WorkNode(bounds_);
    for (int i = 0; i < objects.size(); ++i)
      work_root.objects.push_back(objects[i]);
    work_root.node_index = 0;
    work_list.push_back(work_root);
    next_list.clear();
    // This is a breadth first build. There are two work lists: work_list
    // and next_list.  The work_list gets swapped when empty whilecnext_list
    // fills up. Each time this happens, one level has been completed.
    while (!work_list.empty()) {
      BuildLevel(work_list, next_list, depth);
      work_list.swap(next_list);
      ++depth;
    }
  }
};
}
#endif /* OCTREE_HPP_ */
