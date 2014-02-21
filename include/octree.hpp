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
  Octree() :
      OctreeBase(), nodes_(), scene_objects_(), bounds_() {
  }
  virtual ~Octree() {
    nodes_.clear();
    scene_objects_.clear();
  }
  virtual BoundingBox GetBounds() const {
    return bounds_;
  }
  typedef std::vector<const SceneObject*> ObjectVector;
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
  std::vector<EncodedNode> nodes_;
  ObjectVector scene_objects_;
  BoundingBox bounds_;
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
  void BuildLeaf(OctNode& node, WorkNode& work_node) {
    node.set_offset(scene_objects_.size());
    node.set_size(work_node.objects.size());
    //std::cout << "BuildLeaf node = " << node << std::endl;
    while (!work_node.objects.empty()) {
      scene_objects_.push_back(work_node.objects.back());
      work_node.objects.pop_back();
    }
  }
  void BuildInternal(OctNode& node, WorkNode& work_node, WorkList& next_list,
      uint32_t depth) {
    //std::cout << "BuildInternal node = " << node << " depth = " << depth << std::endl;
    WorkNode child_work_nodes[8]; // process children tentatively
    node.set_offset(nodes_.size()); // children will have nodes pushed
    //std::cout << "bounds = " << work_node.bounds << "\n";
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
      //std::cout << "child_work_nodes[" << j << "].objects.size() = "
      //    << child_work_nodes[j].objects.size() << "\n";
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
    //std::cout << "BuildLevel depth = " << depth << std::endl;
    while (!work_list.empty()) {
      WorkNode work_node = work_list.back();
      work_list.pop_back();
      OctNode node = DecodeNode(nodes_[work_node.node_index]);
      if (node.IsLeaf())
        BuildLeaf(node, work_node);
      else
        BuildInternal(node, work_node, next_list, depth);
      //std::cout << "BuildLevel node pre save = " << node << "\n";
      nodes_[work_node.node_index] = EncodeNode(node);
      //std::cout << "BuildLeaf node post save = "
      //    << DecodeNode(nodes_[work_node.node_index]) << "\n";
    }
    //std::cout << "next_list.size() =" << next_list.size() << "\n";
  }
  void BuildTree(WorkNode& work_root) {
    // compute bounds
    //std::cout << "::: bounds_ = " << bounds_ << "\n";
    for (uint32_t i = 0; i < work_root.objects.size(); ++i) {
      //BoundingBox bounds = work_root.objects[i]->GetBounds();
      //std::cout << "bounds = " << bounds << "\n";
      bounds_ = bounds_.Join(work_root.objects[i]->GetBounds());
      //std::cout << "--> bounds_ = " << bounds_ << "\n";
    }
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
      std::cout << "level = " << depth << "\n";
      ++depth;
    }
    std::cout << "num internal nodes = " << GetNumInternal() << "\n";
    std::cout << "num leaves = " << GetNumLeaves() << "\n";
    std::cout << "num object refs = " << GetNumObjects() << "\n";
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
