/*
 * kdnode64.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: randallsmith
 */
#include <stdint.h>
#include <iostream>
#include "kdnode64.hpp"
namespace ray {
EncodedKdNode64::EncodedKdNode64() {
  for (int i = 0; i < 8; ++i)
    data[i] = 0;
}

EncodedKdNode64::EncodedKdNode64(const EncodedKdNode64& node) {
  for (int i = 0; i < 8; ++i)
    data[i] = node.data[i];
}

EncodedKdNode64& EncodedKdNode64::operator =(const EncodedKdNode64& node) {
  if (this == &node)
    return *this;
  for (int i = 0; i < 8; ++i)
    data[i] = node.data[i];
  return *this;
}

KdNode64::NodeType EncodedKdNode64::GetType() const {
  return static_cast<KdNode64::NodeType>((data[0] & 0xC0) >> 6);
}

uint32_t EncodedKdNode64::GetOffset() const {
  uint32_t offset = (data[0] & 0x0F) << 24;
  uint32_t byte = 0x0;
  for (uint32_t i = 1; i < 4; ++i) {
    byte = static_cast<uint32_t>(data[i]);
    offset = offset | (byte << ((i - 1) * 8));
  }
  return offset;
}

uint32_t EncodedKdNode64::GetSize() const {
  return (data[0] & 0x10) >> 4;
}

uint32_t EncodedKdNode64::GetIndex() const {
  return (data[0] & 0x20) >> 5;
}

float EncodedKdNode64::GetSplitValue() const {
  float value = 0.0f;
  float* src = reinterpret_cast<float *>(&data[4]);
  float* dest = &value;
  *dest = *src;
  return value;
}

void EncodedKdNode64::SetType(KdNode64::NodeType type) {
  u_char mask = static_cast<u_char>(type);
  mask = (mask << 6) & 0xC0;
  data[0] = (0x3F & data[0]) | mask;
}

void EncodedKdNode64::SetIndex(uint32_t index) {
  u_char mask = static_cast<u_char>(index);
  mask = (mask << 5) & 0x20;
  data[0] = (0xDF & data[0]) | mask;
}

void EncodedKdNode64::SetSize(uint32_t size) {
  u_char mask = static_cast<u_char>(size);
  mask = (mask << 4) & 0x10;
  data[0] = (0xEF & data[0]) | mask;
}

void EncodedKdNode64::SetOffset(uint32_t offset) {
  uint32_t byte = 0x0;
  uint32_t shift = 0;
  for (uint32_t i = 1; i < 4; ++i) {
    shift = (i - 1) * 8;
    byte = (offset >> shift) & 0x000000FF;
    data[i] = static_cast<u_char>(byte);
  }
  data[0] = (data[0] & 0xF0) | ((offset & 0x0F000000) >> 24);
}

void EncodedKdNode64::SetSplitValue(float value) {
}

std::ostream& operator<<(std::ostream& out, const EncodedKdNode64& node) {
  for (uint32_t i = 0; i < 8; ++i) {
    for (uint32_t j = 0; j < 8; ++j)
      out << ((0x1 << (7 - j)) & node.data[i] ? '1' : '0');
    out << ' ';
  }
  return out;
}

KdNode64::KdNode64() {
}

KdNode64::KdNode64(const KdNode64& node) {
}

KdNode64::KdNode64(const KdNode64::NodeType& node_type, uint32_t node_size,
    uint32_t node_offset, float split_value) {
}

uint32_t KdNode64::index() const {
  return index_;
}

void KdNode64::set_index(uint32_t index) {
  index_ = index;
}

float KdNode64::split_value() const {
  return split_value_;
}

void KdNode64::set_split_value(float value) {
  split_value_ = value;
}

bool KdNode64::operator ==(const KdNode64& node) const {
}

KdNode64&KdNode64::operator=(const KdNode64&node) {
  if (this == &node)
    return *this;
  type_ = node.type_;
  size_ = node.size_;
  offset_ = node.offset_;
  return *this;
}

KdNodeFactory64::KdNodeFactory64() {
}

KdNodeFactory64 & KdNodeFactory64::GetInstance() {
  static KdNodeFactory64 instance;
  static bool is_initialized = false;
  if (!is_initialized) {
    instance = KdNodeFactory64();
  }
  return instance;
}

KdNode64 KdNodeFactory64::CreateOctNode(const EncodedKdNode64& encoded) const {
  KdNode64 node;
  node.set_size(encoded.GetSize());
  node.set_type(encoded.GetType());
  node.set_offset(encoded.GetOffset());
  return node;
}

KdNode64 KdNodeFactory64::CreateLeaf(uint32_t octant) const {
  return KdNode64(KdNode64::kLeaf, octant, 0, 0);
}

KdNode64 KdNodeFactory64::CreateInternal(uint32_t index) const {
  return KdNode64(KdNode64::kSplitX, index, 0, 0);
}

EncodedKdNode64 KdNodeFactory64::CreateEncodedNode(const KdNode64& node) const {
  EncodedKdNode64 encoded;
  encoded.SetType(node.type());
  encoded.SetOffset(node.offset());
  encoded.SetSize(node.size());
  return encoded;
}

uint32_t KdNode64::offset() const {
  return offset_;
}

void KdNode64::set_offset(uint32_t offset) {
  offset_ = offset;
}

uint32_t KdNode64::size() const {
  return size_;
}

void KdNode64::set_size(uint32_t size) {
  size_ = size;
}

KdNode64::NodeType KdNode64::type() const {
  return type_;
}

void KdNode64::set_type(NodeType type) {
  type_ = type;
}

bool KdNode64::IsLeaf() const {
  return type_ == kLeaf;
}

bool KdNode64::IsInternal() const {
  return type_ != kLeaf;
}

bool KdNode64::IsLeft() const {
  return index() == 0;
}

bool KdNode64::IsRight() const {
  return index() == 1;
}

uint32_t KdNode64::GetNumChildren() const {
  return static_cast<uint32_t>(IsInternal()) & (size() + 1);
}

std::ostream& operator<<(std::ostream& out, const KdNode64&node) {
  return out;
}
} // namespace ray

