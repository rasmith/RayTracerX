/*
 * sah_octnode.hpp
 *
 *  Created on: Mar 5, 2014
 *      Author: randallsmith
 */

#ifndef SAH_OCTNODE_HPP_
#define SAH_OCTNODE_HPP_
#include <sys/types.h>
#include <stdint.h>
#include <ostream>
#include "types.hpp"
namespace ray {
class SAHOctNode {
public:
  enum NodeType {
    kInternal = 0, kLeaf = 1
  };
  SAHOctNode();
  SAHOctNode(const SAHOctNode& node);
  SAHOctNode(const SAHOctNode::NodeType& node_type, uint32_t node_octant,
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
  glm::vec3 point() const;
  void set_point(const glm::vec3& point);
  bool operator==(const SAHOctNode& node) const;
  SAHOctNode& operator=(const SAHOctNode& node);
private:
  NodeType type_;
  uint32_t octant_;
  uint32_t size_;
  uint32_t offset_;
  glm::vec3 point_;
};

std::ostream& operator<<(std::ostream& out, const SAHOctNode& scene);

// data[0]: top bit is type
//      : next three bits indicate octant
//      : bottom four bits unused
// data[1..4]: offset into corresponding array, 32 bits, max size 4,294,967,296
// data[5..7]: node size, 24 bits, max size 16,777,216, empty nodes not stored
// data[8..11]: point_[0]
// data[12..15]: point_[1]
// data[16..19]: point_[2]
struct SAHEncodedNode {
  u_char data[20];
  SAHEncodedNode();
  SAHEncodedNode(const SAHEncodedNode& node);
  SAHEncodedNode& operator=(const SAHEncodedNode& node);
  SAHOctNode::NodeType GetType() const;
  uint32_t GetOctant() const;
  uint32_t GetSize() const;
  uint32_t GetOffset() const;
  glm::vec3 GetPoint() const;
  void SetType(SAHOctNode::NodeType type);
  void SetOctant(uint32_t octant);
  void SetSize(uint32_t size);
  void SetOffset(uint32_t offset);
  void SetPoint(const glm::vec3& point);
};

std::ostream& operator<<(std::ostream& out, const SAHEncodedNode& scene);

class SAHOctNodeFactory {
public:
  static SAHOctNodeFactory& GetInstance();
  SAHOctNode CreateOctNode(const SAHEncodedNode& encoded) const;
  SAHOctNode CreateLeaf(uint32_t octant) const;
  SAHOctNode CreateInternal(uint32_t octant) const;
  SAHEncodedNode CreateEncodedNode(const SAHOctNode& node) const;
private:
  SAHOctNodeFactory();
};

} // namespace ray
#endif /* SAH_OCTNODE_HPP_ */
