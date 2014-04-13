/*
 * SAHOctNode.cpp
 *
 *  Created on: Mar 5, 2014
 *      Author: randallsmith
 */
#include "sah_octnode.hpp"
namespace ray {
SAHEncodedNode::SAHEncodedNode() {
  for (int i = 0; i < 20; ++i)
    data[i] = 0;
}

SAHEncodedNode::SAHEncodedNode(const SAHEncodedNode& node) {
  for (int i = 0; i < 20; ++i)
    data[i] = node.data[i];
}

SAHEncodedNode& SAHEncodedNode::operator =(const SAHEncodedNode& node) {
  if (this == &node)
    return *this;
  for (int i = 0; i < 20; ++i)
    data[i] = node.data[i];
  return *this;
}

SAHOctNode::NodeType SAHEncodedNode::GetType() const {
  return static_cast<SAHOctNode::NodeType>((data[0] & 0x80) >> 7);
}

uint32_t SAHEncodedNode::GetOctant() const {
  return static_cast<uint32_t>((data[0] & 0x70) >> 4);
}

uint32_t SAHEncodedNode::GetSize() const {
  uint32_t size = 0x0;
  uint32_t byte = 0x0;
  for (uint32_t i = 5; i < 8; ++i) {
    byte = static_cast<uint32_t>(data[i]);
    size = size | (byte << ((i - 5) * 8));
  }
  return size;
}
uint32_t SAHEncodedNode::GetOffset() const {
  uint32_t offset = 0x0;
  uint32_t byte = 0x0;
  for (uint32_t i = 1; i < 5; ++i) {
    byte = static_cast<uint32_t>(data[i]);
    offset = offset | (byte << ((i - 1) * 8));
  }
  return offset;
}

glm::vec3 SAHEncodedNode::GetPoint() const {
  glm::vec3 point;
  u_char* dest = reinterpret_cast<u_char *>(&point[0]);
  const u_char* src = &data[8];
  uint32_t num_bytes = sizeof(float) * 3;
  for (uint32_t i = 0; i < num_bytes; ++i)
    dest[i] = src[i];
  return point;
}

void SAHEncodedNode::SetType(SAHOctNode::NodeType type) {
  u_char mask = static_cast<u_char>(type);
  mask = (mask << 7) & 0x80;
  data[0] = (0x7F & data[0]) | mask;
}

void SAHEncodedNode::SetOctant(uint32_t octant) {
  u_char mask = static_cast<u_char>(octant);
  mask = (mask << 4);
  data[0] = (0x8F & data[0]) | mask;
}

void SAHEncodedNode::SetSize(uint32_t size) {
  uint32_t byte = 0x0;
  uint32_t shift = 0;
  for (uint32_t i = 5; i < 8; ++i) {
    shift = (i - 5) * 8;
    byte = (size >> shift) & 0x000000FF;
    data[i] = static_cast<u_char>(byte);
  }
}

void SAHEncodedNode::SetOffset(uint32_t offset) {
  uint32_t byte = 0x0;
  uint32_t shift = 0;
  for (uint32_t i = 1; i < 5; ++i) {
    shift = (i - 1) * 8;
    byte = (offset >> shift) & 0x000000FF;
    data[i] = static_cast<u_char>(byte);
  }
}

void SAHEncodedNode::SetPoint(const glm::vec3& point) {
  const u_char* src = reinterpret_cast<const u_char*>(&point[0]);
  u_char* dest = &data[8];
  uint32_t num_bytes = sizeof(float) * 3;
  for (uint32_t i = 0; i < num_bytes; ++i)
    dest[i] = src[i];
}

std::ostream& operator<<(std::ostream& out, const SAHEncodedNode& node) {
  for (uint32_t i = 0; i < 20; ++i) {
    for (uint32_t j = 0; j < 20; ++j)
      out << ((0x1 << (19 - j)) & node.data[i] ? '1' : '0');
    out << ' ';
  }
  return out;
}

SAHOctNode::SAHOctNode() :
    type_(kInternal), octant_(0), size_(0), offset_(0) {
}

SAHOctNode::SAHOctNode(const SAHOctNode::NodeType& node_type,
    uint32_t node_octant, uint32_t node_size, uint32_t node_offset) :
    type_(node_type), octant_(node_octant), size_(node_size),
        offset_(node_offset) {
}

SAHOctNode::SAHOctNode(const SAHOctNode&node) :
    type_(node.type_), octant_(node.octant_), size_(node.size_),
        offset_(node.offset_), point_(node.point_) {
}

bool SAHOctNode::operator==(const SAHOctNode& node) const {
  return type() == node.type() && octant() == node.octant()
      && size() == node.size() && offset() == node.offset()
      && point() == node.point();
}

SAHOctNode&SAHOctNode::operator=(const SAHOctNode&node) {
  if (this == &node)
    return *this;
  type_ = node.type_;
  octant_ = node.octant_;
  size_ = node.size_;
  offset_ = node.offset_;
  point_ = node.point_;
  return *this;
}

SAHOctNodeFactory::SAHOctNodeFactory() {
}

SAHOctNodeFactory & SAHOctNodeFactory::GetInstance() {
  static SAHOctNodeFactory instance;
  static bool is_initialized = false;
  if (!is_initialized) {
    instance = SAHOctNodeFactory();
  }
  return instance;
}

SAHOctNode SAHOctNodeFactory::CreateOctNode(
    const SAHEncodedNode& encoded) const {
  SAHOctNode node;
  node.set_size(encoded.GetSize());
  node.set_octant(encoded.GetOctant());
  node.set_type(encoded.GetType());
  node.set_offset(encoded.GetOffset());
  node.set_point(encoded.GetPoint());
  return node;
}

SAHOctNode SAHOctNodeFactory::CreateLeaf(uint32_t octant) const {
  return SAHOctNode(SAHOctNode::kLeaf, octant, 0, 0);
}

SAHOctNode SAHOctNodeFactory::CreateInternal(uint32_t octant) const {
  return SAHOctNode(SAHOctNode::kInternal, octant, 0, 0);
}

SAHEncodedNode SAHOctNodeFactory::CreateEncodedNode(
    const SAHOctNode& node) const {
  SAHEncodedNode encoded;
  encoded.SetType(node.type());
  encoded.SetOctant(node.octant());
  encoded.SetOffset(node.offset());
  encoded.SetSize(node.size());
  encoded.SetPoint(node.point());
  return encoded;
}

uint32_t SAHOctNode::octant() const {
  return octant_;
}

void SAHOctNode::set_octant(uint32_t octant) {
  octant_ = octant;
}

uint32_t SAHOctNode::offset() const {
  return offset_;
}

void SAHOctNode::set_offset(uint32_t offset) {
  offset_ = offset;
}

uint32_t SAHOctNode::size() const {
  return size_;
}

void SAHOctNode::set_size(uint32_t size) {
  size_ = size;
}

SAHOctNode::NodeType SAHOctNode::type() const {
  return type_;
}

void SAHOctNode::set_type(NodeType type) {
  type_ = type;
}

glm::vec3 SAHOctNode::point() const {
  return point_;
}

void SAHOctNode::set_point(const glm::vec3& point) {
  point_ = point;
}

bool SAHOctNode::IsLeaf() const {
  return type_ == kLeaf;
}

bool SAHOctNode::IsInternal() const {
  return type_ == kInternal;
}

std::ostream& operator<<(std::ostream& out, const SAHOctNode&node) {
  if (node.IsLeaf())
    out << "L:";
  else
    out << "N:";
  out << "[" << node.octant() << "] #" << node.size() << " @" << node.offset();
  return out;
}
}

