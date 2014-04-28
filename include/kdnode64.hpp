/*
 * kdnode64.hpp
 *
 *  Created on: Apr 22, 2014
 *      Author: randallsmith
 */

#ifndef KDNODE64_HPP_
#define KDNODE64_HPP_

#include <stdint.h>
#include <sys/types.h>
#include <ostream>
#include "types.hpp"
namespace ray {
class KdNode64 {
public:
  enum NodeType {
    kSplitX = 0, kSplitY, kSplitZ, kLeaf
  };
  KdNode64();
  KdNode64(const KdNode64& node);
  KdNode64(const KdNode64::NodeType& node_type, uint32_t order,
      uint32_t node_offset, uint32_t num_children, float split_value);
  KdNode64(uint32_t order, uint32_t node_offset, uint32_t num_objects);
  bool IsLeaf() const;
  bool IsInternal() const;
  bool IsLeft() const;
  bool IsRight() const;
  uint32_t num_children() const;
  void set_num_children(uint32_t num_children);
  uint32_t num_objects() const;
  void set_num_objects(uint32_t num_objects);
  NodeType type() const;
  void set_type(NodeType type);
  void set_type(uint32_t type);
  uint32_t order() const;
  void set_order(uint32_t order);
  uint32_t offset() const;
  void set_offset(uint32_t offset);
  float split_value() const;
  void set_split_value(float value);
  bool operator==(const KdNode64& node) const;
  KdNode64& operator=(const KdNode64& node);
private:
  NodeType type_;
  uint32_t order_;
  uint32_t num_children_;
  uint32_t num_objects_;
  uint32_t offset_;
  float split_value_;
};

std::ostream& operator<<(std::ostream& out, const KdNode64& node);

// data[0]: bits 7-6: type (split_x, split_y, split_z, leaf)
//      : bit 5: index (left or right)
//      : bit 4: num children (1 or 2 children)
//      : bits 3-0: offset top bits
// data[1..3]: offset lower bits (offset has 28 bits)
// data[4..7]: split value or object count
struct EncodedKdNode64 {
  u_char data[8];
  EncodedKdNode64();
  EncodedKdNode64(const EncodedKdNode64& node);
  EncodedKdNode64& operator=(const EncodedKdNode64& node);
  KdNode64::NodeType GetType() const;
  uint32_t GetOrder() const;
  uint32_t GetNumObjects() const;
  uint32_t GetNumChildren() const;
  uint32_t GetOffset() const;
  float GetSplitValue() const;
  void SetType(KdNode64::NodeType type);
  void SetOrder(uint32_t order);
  void SetNumObjects(uint32_t num_objects);
  void SetNumChildren(uint32_t num_children);
  void SetOffset(uint32_t offset);
  void SetSplitValue(float value);
};

std::ostream& operator<<(std::ostream& out, const EncodedKdNode64& scene);

class KdNode64Factory {
public:
  static KdNode64Factory& GetInstance();
  KdNode64 CreateNode(const EncodedKdNode64& encoded) const;
  KdNode64 CreateLeaf(uint32_t octant) const;
  KdNode64 CreateInternal(uint32_t octant) const;
  EncodedKdNode64 CreateEncodedNode(const KdNode64& node) const;
private:
  KdNode64Factory();
};
} // namespace ray
#endif /* KDNODE64_HPP_ */
