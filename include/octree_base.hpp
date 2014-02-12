/*
 * octree_base.hpp
 *
 *  Created on: Feb 5, 2014
 *      Author: agrippa
 */

#ifndef OCTREE_BASE_HPP_
#define OCTREE_BASE_HPP_
#include <stdint.h>
#include "shape.hpp"
namespace ray {
class OctNode {
public:
  enum NodeType {
    kInternal = 0, kLeaf = 1
  };
  OctNode();
  OctNode(const OctNode::NodeType& node_type, uint32_t node_octant,
      uint32_t node_size, uint32_t node_offset);
  bool IsLeaf() const;
  bool IsInternal() const;
  uint32_t octant() const;
  void set_octant(uint32_t octant);
  uint32_t offset() const;
  void set_offset(uint32_t offset);
  uint32_t size() const;
  void set_size(uint32_t size);
  NodeType type() const;
  void set_type(NodeType type);
private:
  NodeType type_;
  uint32_t octant_;
  uint32_t size_;
  uint32_t offset_;
};
// data[0]: top bit is type
//      : next three bits indicate octant
//      : bottom four bits unused
// data[1]: size of node, 8 bits, max size of 256, empty nodes not stored
// data[2..5]: offset into corresponding array, 32 bits
struct EncodedNode {
  u_char data[8];
  OctNode::NodeType GetType() const;
  uint32_t GetOctant() const;
  uint32_t GetSize() const;
  uint32_t GetOffset() const;
  void SetType(OctNode::NodeType type);
  void SetOctant(uint32_t octant);
  void SetSize(uint32_t size);
  void SetOffset(uint32_t offset);
};
class OctNodeFactory {
public:
  static OctNodeFactory& GetInstance();
  OctNode CreateOctNode(const EncodedNode& encoded) const;
  OctNode CreateLeaf(uint32_t octant) const;
  OctNode CreateInternal(uint32_t octant) const;
  EncodedNode CreateEncodedNode(const OctNode& node) const;
private:
  OctNodeFactory();
};
class OctreeBase: public Shape {
public:
  static const int kMaxDepth;
  static const int kMaxLeafSize;
private:
  OctreeBase();
  uint32_t PointToOctant(const BoundingBox& bounds,
      const glm::vec3& point) const;
  BoundingBox GetChildBounds(const BoundingBox& bounds, uint32_t octant) const;
  virtual bool Intersect(const Ray& ray, Isect& isect) const;
  virtual bool Traverse(const OctNode& node, const BoundingBox& bounds,
      const Ray& ray, Isect& isect, int depth) const;
  bool TraverseStackless(const OctNode& node, const BoundingBox& bounds,
      const Ray& ray, Isect& isect) const;
  virtual OctNode GetIthChildOf(const OctNode& node, uint32_t index) const = 0;
  virtual OctNodeFactory& GetNodeFactory() const;
  virtual bool IntersectLeaf(const OctNode& leaf, const BoundingBox& bounds,
      const Ray& ray, Isect& isect) const = 0;
  virtual void IntersectChildren(const OctNode& node, const BoundingBox& bounds,
      const Ray& ray, OctNode* children, BoundingBox* child_bounds,
      uint32_t& count) const;
  virtual OctNode GetRoot() const = 0;
  virtual BoundingBox GetBounds() const = 0;
  virtual void Print(std::ostream& out) const;
  virtual void PrintNode(std::ostream& out, const OctNode& node,
      int depth) const;
};
} // namespace ray
#endif
