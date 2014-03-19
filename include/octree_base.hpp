/*
 * octree_base.hpp
 *
 *  Created on: Feb 5, 2014
 *      Author: agrippa
 */

#ifndef OCTREE_BASE_HPP_
#define OCTREE_BASE_HPP_
#include <stdint.h>
#include <sys/types.h>
#include "scene.hpp"
#include "shape.hpp"
namespace ray {
class Accelerator: public SceneShape {
public:
  virtual ~Accelerator();
  bool trace() const;
  void set_trace(bool trace);
protected:
  Accelerator();
  bool trace_;
};

template<class OctNode, class EncodedNode, class OctNodeFactory,
    int max_leaf_size = 32, int max_depth = 8>
class OctreeBase: public Accelerator {
public:
  virtual bool Intersect(const Ray& ray, Isect& isect) const {
    return Traverse(GetRoot(), GetBounds(), ray, isect, 0);
  }

  virtual void Print(std::ostream& out) const {
    std::vector<OctNode> nodes;
    std::vector<int> depths;
    std::vector<BoundingBox> bounds;
    nodes.clear();
    depths.clear();
    bounds.clear();
    nodes.push_back(GetRoot());
    bounds.push_back(GetBounds());
    depths.push_back(0);
    while (!nodes.empty()) {
      OctNode node = nodes.back();
      int depth = depths.back();
      BoundingBox bbox = bounds.back();
      nodes.pop_back();
      bounds.pop_back();
      depths.pop_back();
      PrintNode(out, node, bbox, depth);
      if (node.IsInternal()) {
        for (uint32_t i = 0; i < node.size(); ++i) {
          OctNode child = GetIthChildOf(node, i);
          nodes.push_back(child);
          bounds.push_back(GetChildBounds(node, bbox, child.octant()));
          depths.push_back(depth + 1);
        }
      }
    }
  }
  virtual OctNode GetRoot() const = 0;
  virtual uint32_t PointToOctant(const OctNode& node, const BoundingBox& bounds,
      const glm::vec3& point) const = 0;
  virtual BoundingBox GetChildBounds(const OctNode& node,
      const BoundingBox& bounds, uint32_t octant) const = 0;
  virtual BoundingBox GetBounds() const = 0;
  virtual OctNode GetIthChildOf(const OctNode& node, uint32_t index) const = 0;
protected:
  virtual bool IntersectLeaf(const OctNode& leaf, const Ray& ray,
      Isect& isect) const = 0;

  OctreeBase() :
      Accelerator() {
  }

  bool Traverse(const OctNode& node, const BoundingBox& bounds, const Ray& ray,
      Isect& isect, uint32_t depth) const {
    if (depth > max_depth) // check depth first
      return false;
    float t_near, t_far;
    if (!bounds.Intersect(ray, t_near, t_far)) // check bounds next
      return false;
    if (node.IsLeaf()) { // is this a leaf?
      bool hit = IntersectLeaf(node, ray, isect);
      return hit;
    }
    OctNode children[4];    // can hit at most four children
    BoundingBox child_bounds[4];
    uint32_t count = 0;
    IntersectChildren(node, bounds, ray, &children[0], &child_bounds[0], count);
    bool hit = false;
    for (uint32_t i = 0; i < count && !hit; ++i)
      hit = Traverse(children[i], child_bounds[i], ray, isect, depth + 1);
    return hit;
  }

  virtual OctNodeFactory& GetNodeFactory() const {
    return OctNodeFactory::GetInstance();
  }

  ///////
  //
  // IntersectChildren
  //
  //  Intersect children of given node and return in sorted order
  //  that the given ray hit them along with child bounding boxes.
  //  Can hit at most four (4) children.  Arrays passed in are
  //  assumed to be of length four (4).
  //
  //////
  struct SortHolder {
    float t_near;
    float t_far;
    OctNode child;
    BoundingBox bounds;
    SortHolder() :
        t_near(0.0f), t_far(0.0f), child(OctNode()), bounds(BoundingBox()) {
    }

    SortHolder(const SortHolder& h) :
        t_near(h.t_near), t_far(h.t_far), child(h.child), bounds(h.bounds) {
    }

    explicit SortHolder(float t0, float t1, const OctNode& n,
        const BoundingBox& b) :
        t_near(t0), t_far(t1), child(n), bounds(b) {
    }

    bool operator<(const SortHolder& s) const {
      return t_near < s.t_near;
    }
  };

  virtual void IntersectChildren(const OctNode& node, const BoundingBox& bounds,
      const Ray& ray, OctNode* children, BoundingBox* child_bounds,
      uint32_t& count) const {
    float t_near;
    float t_far;
    SortHolder h[4];
    count = 0;
    for (uint32_t i = 0; count < 4 && i < node.size(); ++i) {
      children[count] = GetIthChildOf(node, i);
      child_bounds[count] = GetChildBounds(node, bounds,
          children[count].octant());
      if (child_bounds[count].Intersect(ray, t_near, t_far)) {
        h[count] = SortHolder(t_near, t_far, children[count],
            child_bounds[count]);
        if (trace_)
          std::cout << "i = " << i << " t_near = " << t_near << " t_far = "
              << t_far << " child = " << children[count] << "\n";
        ++count;
      }
    }
    if (trace_)
      std::cout << "Sort children:\n";
    std::sort(&h[0], &h[0] + count);
    for (uint32_t i = 0; i < count; ++i) {
      children[i] = h[i].child;
      child_bounds[i] = h[i].bounds;
      if (trace_)
        std::cout << "i = " << i << " t_near = " << h[i].t_near << " t_far = "
            << h[i].t_far << " node = " << children[i] << "\n";
    }
  }

  virtual void PrintNode(std::ostream& out, const OctNode& node,
      const BoundingBox& bbox, int depth) const {
    for (int i = 0; i < depth; ++i)
      out << " ";
    out << node << " " << bbox << "\n";
  }
};
} // namespace ray
#endif
