/*
 * octnode64.cpp
 *
 *  Created on: Mar 5, 2014
 *      Author: randallsmith
 */
#include "octnode64.hpp"
namespace ray {
EncodedNode64::EncodedNode64() {
  for (int i = 0; i < 8; ++i)
    data[i] = 0;
}

EncodedNode64::EncodedNode64(const EncodedNode64& node) {
  for (int i = 0; i < 8; ++i)
    data[i] = node.data[i];
}

EncodedNode64& EncodedNode64::operator =(const EncodedNode64& node) {
  if (this == &node)
    return *this;
  for (int i = 0; i < 8; ++i)
    data[i] = node.data[i];
  return *this;
}

typename OctNode64::NodeType EncodedNode64::GetType() const {
  return static_cast<OctNode64::NodeType>((data[0] & 0x80) >> 7);
}

uint32_t EncodedNode64::GetOctant() const {
  return static_cast<uint32_t>((data[0] & 0x70) >> 4);
}

uint32_t EncodedNode64::GetSize() const {
  uint32_t size = 0x0;
  uint32_t byte = 0x0;
  for (uint32_t i = 5; i < 8; ++i) {
    byte = static_cast<uint32_t>(data[i]);
    size = size | (byte << ((i - 5) * 8));
  }
  return size;
}
uint32_t EncodedNode64::GetOffset() const {
  uint32_t offset = 0x0;
  uint32_t byte = 0x0;
  for (uint32_t i = 1; i < 5; ++i) {
    byte = static_cast<uint32_t>(data[i]);
    offset = offset | (byte << ((i - 1) * 8));
  }
  return offset;
}

void EncodedNode64::SetType(OctNode64::NodeType type) {
  u_char mask = static_cast<u_char>(type);
  mask = (mask << 7) & 0x80;
  data[0] = (0x7F & data[0]) | mask;
}

void EncodedNode64::SetOctant(uint32_t octant) {
  u_char mask = static_cast<u_char>(octant);
  mask = (mask << 4);
  data[0] = (0x8F & data[0]) | mask;
}

void EncodedNode64::SetSize(uint32_t size) {
  uint32_t byte = 0x0;
  uint32_t shift = 0;
  for (uint32_t i = 5; i < 8; ++i) {
    shift = (i - 5) * 8;
    byte = (size >> shift) & 0x000000FF;
    data[i] = static_cast<u_char>(byte);
  }
}

void EncodedNode64::SetOffset(uint32_t offset) {
  uint32_t byte = 0x0;
  uint32_t shift = 0;
  for (uint32_t i = 1; i < 5; ++i) {
    shift = (i - 1) * 8;
    byte = (offset >> shift) & 0x000000FF;
    data[i] = static_cast<u_char>(byte);
  }
}

std::ostream& operator<<(std::ostream& out, const EncodedNode64& node) {
  for (uint32_t i = 0; i < 8; ++i) {
    for (uint32_t j = 0; j < 8; ++j)
      out << ((0x1 << (7 - j)) & node.data[i] ? '1' : '0');
    out << ' ';
  }
  return out;
}

OctNode64::OctNode64() :
    type_(kInternal), octant_(0), size_(0), offset_(0) {
}

OctNode64::OctNode64(const OctNode64::NodeType& node_type, uint32_t node_octant,
    uint32_t node_size, uint32_t node_offset) :
    type_(node_type), octant_(node_octant), size_(node_size),
        offset_(node_offset) {
}

OctNode64::OctNode64(const OctNode64&node) :
    type_(node.type_), octant_(node.octant_), size_(node.size_),
        offset_(node.offset_) {
}

bool OctNode64::operator==(const OctNode64&node) const {
  return type() == node.type() && octant() == node.octant()
      && size() == node.size() && offset() == node.offset();
}

OctNode64&OctNode64::operator=(const OctNode64&node) {
  if (this == &node)
    return *this;
  type_ = node.type_;
  octant_ = node.octant_;
  size_ = node.size_;
  offset_ = node.offset_;
  return *this;
}

OctNodeFactory64::OctNodeFactory64() {
}

OctNodeFactory64 & OctNodeFactory64::GetInstance() {
  static OctNodeFactory64 instance;
  static bool is_initialized = false;
  if (!is_initialized) {
    instance = OctNodeFactory64();
  }
  return instance;
}

OctNode64 OctNodeFactory64::CreateOctNode(const EncodedNode64& encoded) const {
  OctNode64 node;
  node.set_size(encoded.GetSize());
  node.set_octant(encoded.GetOctant());
  node.set_type(encoded.GetType());
  node.set_offset(encoded.GetOffset());
  return node;
}

OctNode64 OctNodeFactory64::CreateLeaf(uint32_t octant) const {
  return OctNode64(OctNode64::kLeaf, octant, 0, 0);
}

OctNode64 OctNodeFactory64::CreateInternal(uint32_t octant) const {
  return OctNode64(OctNode64::kInternal, octant, 0, 0);
}

EncodedNode64 OctNodeFactory64::CreateEncodedNode(const OctNode64& node) const {
  EncodedNode64 encoded;
  encoded.SetType(node.type());
  encoded.SetOctant(node.octant());
  encoded.SetOffset(node.offset());
  encoded.SetSize(node.size());
  return encoded;
}

uint32_t OctNode64::octant() const {
  return octant_;
}

void OctNode64::set_octant(uint32_t octant) {
  octant_ = octant;
}

uint32_t OctNode64::offset() const {
  return offset_;
}

void OctNode64::set_offset(uint32_t offset) {
  offset_ = offset;
}

uint32_t OctNode64::size() const {
  return size_;
}

void OctNode64::set_size(uint32_t size) {
  size_ = size;
}

OctNode64::NodeType OctNode64::type() const {
  return type_;
}

void OctNode64::set_type(NodeType type) {
  type_ = type;
}

bool OctNode64::IsLeaf() const {
  return type_ == kLeaf;
}

bool OctNode64::IsInternal() const {
  return type_ == kInternal;
}

std::ostream& operator<<(std::ostream& out, const OctNode64&node) {
  if (node.IsLeaf())
    out << "L:";
  else
    out << "N:";
  out << "[" << node.octant() << "] #" << node.size() << " @" << node.offset();
  return out;
}
}

