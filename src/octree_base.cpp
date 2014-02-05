/*
 * octree_base.cpp
 *
 *  Created on: Feb 5, 2014
 *      Author: agrippa
 */
#include "octree_base.hpp"
namespace ray {
const int OctTreeBase::kMaxDepth = 20;
const int OctTreeBase::kMaxLeafSize = 16;
uint32_t OctTreeBase::PointToOctant(const BoundingBox& bounds,
    const glm::vec3& point) const {
  glm::vec3 center = bounds.GetCenter();
  uint32_t x_bit = (point[0] > center[0]);
  uint32_t y_bit = (point[1] > center[1]);
  uint32_t z_bit = (point[2] > center[2]);
  uint32_t octant = x_bit | (y_bit << 1) | (z_bit << 2);
  return octant;
}
BoundingBox OctTreeBase::GetChildBounds(const BoundingBox& bounds,
    uint32_t octant) const {
  BoundingBox child_bounds;
  return child_bounds;
}
OctNodeFactory& GetNodeFactory() const {
  return OctNodeFactory::GetInstance();
}
OctNode::NodeType EncodedNode::GetType() const {
  return static_cast<OctNode::NodeType>((data[0] & 0x80u) >> 7);
}
uint32_t EncodedNode::GetOctant() const {
  return static_cast<uint32_t>((data[0] & 0x30u) >> 4);
}
uint32_t EncodedNode::GetSize() const {
  return static_cast<uint32_t>(data[1]);
}
uint32_t EncodedNode::GetOffset() const {
  uint32_t offset = 0x0u;
  uint32_t byte = 0x0u;
  for (uint32_t i = 2; i < 6; ++i) {
    byte = static_cast<uint32_t>(data[i]);
    offset = offset | (byte << ((i - 2) * 8));
  }
  return offset;
}
void EncodedNode::SetType(OctNode::NodeType type) {
  u_char mask = static_cast<u_char>(type);
  mask = (mask << 7) & 0x80u;
  data[0] = data[0] | mask;
}
void EncodedNode::SetOctant(uint32_t octant) {
  u_char mask = static_cast<u_char>(octant);
  mask = (mask << 4) & 0x30u;
  data[0] = data[0] | mask;
}
void EncodedNode::SetSize(uint32_t size) {
  u_char mask = static_cast<u_char>(size);
  data[1] = mask;
}
void EncodedNode::SetOffset(uint32_t offset) {
  uint32_t byte = 0x0u;
  uint32_t shift = 0;
  for (uint32_t i = 2; i < 6; ++i) {
    shift = (i - 2) * 8;
    byte = (offset >> shift) & 0x0000000Fu;
    data[i] = static_cast<u_char>(byte);
  }
}
OctNode::OctNode(const OctNode::NodeType& node_type, uint32_t node_octant,
    uint32_t node_size, uint32_t node_offset) :
    type_(node_type), octant_(node_octant), size_(node_size),
        offset_(node_offset) {
}
OctNodeFactory::OctNodeFactory() {
}
OctNodeFactory& OctNodeFactory::GetInstance() {
  static OctNodeFactory instance;
  static bool is_initialized = false;
  if (!is_initialized) {
    instance = OctNodeFactory();
  }
  return instance;
}
OctNode OctNodeFactory::CreateOctNode(const EncodedNode& encoded) const {
  OctNode node;
  uint32_t offset = encoded.GetOffset();
  node.size_ = encoded.GetSize();
  node.octant_ = encoded.GetOctant();
  node.type_ = encoded.GetType();
  node.offset_ = encoded.GetOffset();
  return node;
}
OctNode OctNodeFactory::CreateLeaf(uint32_t octant) const {
  return OctNode(OctNode::kLeaf, octant, 0, 0);
}
OctNode OctNodeFactory::CreateInternal(uint32_t octant) const {
  return OctNode(OctNode::kInternal, octant, 0, 0);
}
OctNode OctNodeFactory::CreateRoot() const {
  return OctNode(OctNode::kInternal, 0, 0, 0);
}
EncodedNode OctNodeFactory::CreateEncodedNode(const OctNode& node) const {
  EncodedNode encoded;
  encoded.SetType(node.type_);
  encoded.SetOctant(node.octant_);
  encoded.SetOffset(node.offset_);
  encoded.SetSize(node.size_);
  return encoded;
}
uint32_t OctNode::octant() const {
  return octant_;
}
void OctNode::set_octant(uint32_t octant) {
  octant_ = octant;
}
uint32_t OctNode::offset() const {
  return offset_;
}
void OctNode::set_offset(uint32_t offset) {
  offset_ = offset;
}
uint32_t OctNode::size() const {
  return size_;
}
void OctNode::set_size(uint32_t size) {
  size_ = size;
}
OctNode::NodeType OctNode::type() const {
  return type_;
}
void OctNode::set_type(NodeType type) {
  type_ = type;
}
bool OctNode::IsLeaf() const {
  return type == kLeaf;
}
bool OctNode::IsInternal() const {
  return type == kInternal;
}
}  // namespace ray
