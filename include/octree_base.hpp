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
class OctNode {
public:
  enum NodeType {
    kInternal = 0, kLeaf = 1
  };
  OctNode();
  OctNode(const OctNode& node);
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
  bool operator==(const OctNode& node) const;
  OctNode& operator=(const OctNode& node);
private:
  NodeType type_;
  uint32_t octant_;
  uint32_t size_;
  uint32_t offset_;
};
std::ostream& operator<<(std::ostream& out, const OctNode& scene);
// data[0]: top bit is type
//      : next three bits indicate octant
//      : bottom four bits unused
// data[1..4]: offset into corresponding array, 32 bits, max size 4,294,967,296
// data[5..7]: node size, 24 bits, max size 16,777,216, empty nodes not stored
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
std::ostream& operator<<(std::ostream& out, const EncodedNode& scene);
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
class Accelerator: public SceneShape {
public:
  virtual ~Accelerator();
  bool trace() const;
  void set_trace(bool trace);
protected:
  Accelerator();
  bool trace_;
};
class OctreeBase: public Accelerator {
public:
  static const uint32_t kMaxDepth;
  static const uint32_t kMaxLeafSize;
  uint32_t PointToOctant(const BoundingBox& bounds,
      const glm::vec3& point) const;
  BoundingBox GetChildBounds(const BoundingBox& bounds, uint32_t octant) const;
  virtual bool Intersect(const Ray& ray, Isect& isect) const;
  virtual void Print(std::ostream& out) const;
protected:
  OctreeBase();
  virtual bool Traverse(const OctNode& node, const BoundingBox& bounds,
      const Ray& ray, Isect& isect, uint32_t depth) const;
  bool TraverseStackless(const OctNode& node, const BoundingBox& bounds,
      const Ray& ray, Isect& isect) const;
  virtual OctNode GetIthChildOf(const OctNode& node, uint32_t index) const = 0;
  virtual OctNodeFactory& GetNodeFactory() const;
  virtual bool IntersectLeaf(const OctNode& leaf, const Ray& ray,
      Isect& isect) const = 0;
  virtual void IntersectChildren(const OctNode& node, const BoundingBox& bounds,
      const Ray& ray, OctNode* children, BoundingBox* child_bounds,
      uint32_t& count) const;
  virtual OctNode GetRoot() const = 0;
  virtual BoundingBox GetBounds() const = 0;
  virtual void PrintNode(std::ostream& out, const OctNode& node,
      const BoundingBox& bbox, int depth) const;
};
} // namespace ray
#endif
