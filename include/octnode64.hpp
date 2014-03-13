/*
 * octnode64.hpp
 *
 *  Created on: Mar 5, 2014
 *      Author: randallsmith
 */

#ifndef OCTNODE64_HPP_
#define OCTNODE64_HPP_
#include <sys/types.h>
#include <stdint.h>
#include <ostream>
#include "types.hpp"
namespace ray {
class OctNode64 {
public:
  enum NodeType {
    kInternal = 0, kLeaf = 1
  };
  OctNode64();
  OctNode64(const OctNode64& node);
  OctNode64(const OctNode64::NodeType& node_type, uint32_t node_octant,
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
  bool operator==(const OctNode64& node) const;
  OctNode64& operator=(const OctNode64& node);
private:
  NodeType type_;
  uint32_t octant_;
  uint32_t size_;
  uint32_t offset_;
};

std::ostream& operator<<(std::ostream& out, const OctNode64& scene);

// data[0]: top bit is type
//      : next three bits indicate octant
//      : bottom four bits unused
// data[1..4]: offset into corresponding array, 32 bits, max size 4,294,967,296
// data[5..7]: node size, 24 bits, max size 16,777,216, empty nodes not stored
struct EncodedNode64 {
  u_char data[8];
  EncodedNode64();
  EncodedNode64(const EncodedNode64& node);
  EncodedNode64& operator=(const EncodedNode64& node);
  OctNode64::NodeType GetType() const;
  uint32_t GetOctant() const;
  uint32_t GetSize() const;
  uint32_t GetOffset() const;
  void SetType(OctNode64::NodeType type);
  void SetOctant(uint32_t octant);
  void SetSize(uint32_t size);
  void SetOffset(uint32_t offset);
};

std::ostream& operator<<(std::ostream& out, const EncodedNode64& scene);

class OctNodeFactory64 {
public:
  static OctNodeFactory64& GetInstance();
  OctNode64 CreateOctNode(const EncodedNode64& encoded) const;
  OctNode64 CreateLeaf(uint32_t octant) const;
  OctNode64 CreateInternal(uint32_t octant) const;
  EncodedNode64 CreateEncodedNode(const OctNode64& node) const;
private:
  OctNodeFactory64();
};

} // namespace ray
#endif /* OCTNODE64_HPP_ */
