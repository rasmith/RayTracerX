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

uint32_t EncodedKdNode64::GetOrder() const {
  return (data[0] & 0x20) >> 5;
}

uint32_t EncodedKdNode64::GetNumChildren() const {
  return (data[0] & 0x10) >> 4;
}

uint32_t EncodedKdNode64::GetNumObjects() const {
  uint32_t num_objects = 0;
  const uint32_t* src = reinterpret_cast<const uint32_t *>(&data[4]);
  uint32_t* dest = &num_objects;
  *dest = *src;
  return num_objects;
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

float EncodedKdNode64::GetSplitValue() const {
  float value = 0.0f;
  const float* src = reinterpret_cast<const float *>(&data[4]);
  float* dest = &value;
  *dest = *src;
  return value;
}

void EncodedKdNode64::SetType(KdNode64::NodeType type) {
  u_char mask = static_cast<u_char>(type);
  mask = (mask << 6) & 0xC0;
  data[0] = (0x3F & data[0]) | mask;
}

void EncodedKdNode64::SetOrder(uint32_t order) {
  u_char mask = static_cast<u_char>(order);
  mask = (mask << 5) & 0x20;
  data[0] = (0xDF & data[0]) | mask;
}

void EncodedKdNode64::SetNumChildren(uint32_t num_children) {
  u_char mask = static_cast<u_char>(num_children);
  mask = (mask << 4) & 0x10;
  data[0] = (0xEF & data[0]) | mask;
}

void EncodedKdNode64::SetNumObjects(uint32_t num_objects) {
  uint32_t* src = &num_objects;
  uint32_t* dest = reinterpret_cast<uint32_t *>(&data[4]);
  *dest = *src;
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
  float* src = &value;
  float* dest = reinterpret_cast<float *>(&data[4]);
  *dest = *src;
}

std::ostream& operator<<(std::ostream& out, const EncodedKdNode64& node) {
  for (uint32_t i = 0; i < 8; ++i) {
    for (uint32_t j = 0; j < 8; ++j)
      out << ((0x1 << (7 - j)) & node.data[i] ? '1' : '0');
    out << ' ';
  }
  return out;
}

KdNode64::KdNode64() :
    type_(kSplitX), order_(0), num_children_(0), num_objects_(0), offset_(0),
        split_value_(0.0f) {
}

KdNode64::KdNode64(const KdNode64& node) :
    type_(node.type_), order_(node.order_), num_children_(node.num_children_),
        num_objects_(node.num_objects_), offset_(node.offset_),
        split_value_(node.split_value_) {
}

KdNode64::KdNode64(const KdNode64::NodeType& type, uint32_t order,
    uint32_t num_children, uint32_t offset, float split_value) :
    type_(type), order_(order), num_children_(num_children), num_objects_(0),
        offset_(offset), split_value_(split_value) {
}

KdNode64::KdNode64(uint32_t order, uint32_t num_objects, uint32_t offset) :
    type_(KdNode64::kLeaf), order_(order), num_children_(0),
        num_objects_(num_objects), offset_(offset), split_value_(0.0f) {
}

bool KdNode64::IsLeaf() const {
  return type_ == kLeaf;
}

bool KdNode64::IsInternal() const {
  return type_ != kLeaf;
}

bool KdNode64::IsLeft() const {
  return order() == 0;
}

bool KdNode64::IsRight() const {
  return order() == 1;
}

uint32_t KdNode64::num_children() const {
  return num_children_;
}

void KdNode64::set_num_children(uint32_t num_children) {
  num_children_ = num_children;
}

uint32_t KdNode64::num_objects() const {
  return num_objects_;
}

void KdNode64::set_num_objects(uint32_t num_objects) {
  num_objects_ = num_objects;
}

KdNode64::NodeType KdNode64::type() const {
  return type_;
}

void KdNode64::set_type(NodeType type) {
  type_ = type;
}

uint32_t KdNode64::order() const {
  return order_;
}

void KdNode64::set_order(uint32_t order) {
  order_ = order;
}

uint32_t KdNode64::offset() const {
  return offset_;
}

void KdNode64::set_offset(uint32_t offset) {
  offset_ = offset;
}

float KdNode64::split_value() const {
  return split_value_;
}

void KdNode64::set_split_value(float value) {
  split_value_ = value;
}

bool KdNode64::operator ==(const KdNode64& node) const {
  return type_ == node.type_ && order_ == node.order_
      && num_children_ == node.num_children_
      && num_objects_ == node.num_objects_ && offset_ == node.offset_
      && split_value_ && node.split_value_;
}

KdNode64&KdNode64::operator=(const KdNode64&node) {
  if (this == &node)
    return *this;
  type_ = node.type_;
  order_ = node.order_;
  num_objects_ = node.num_objects_;
  num_children_ = node.num_children_;
  offset_ = node.offset_;
  split_value_ = node.split_value_;
  return *this;
}

std::ostream& operator<<(std::ostream& out, const KdNode64& node) {
  char char_type[4] = { 'X', 'Y', 'Z', 'L' };
  out << char_type[static_cast<uint32_t>(node.type())] << ":";
  out << "[" << node.order() << "] #";
  if (node.IsLeaf())
    out << node.num_objects();
  else
    out << node.num_children();
  out << " @" << node.offset();
  if (node.IsInternal())
    out << " " << node.split_value();
  return out;
}

KdNode64Factory::KdNode64Factory() {
}

KdNode64Factory & KdNode64Factory::GetInstance() {
  static KdNode64Factory instance;
  static bool is_initialized = false;
  if (!is_initialized)
    instance = KdNode64Factory();
  return instance;
}

KdNode64 KdNode64Factory::CreateNode(const EncodedKdNode64& encoded) const {
  KdNode64 node;
  node.set_type(encoded.GetType());
  node.set_order(encoded.GetOrder());
  node.set_num_objects(encoded.GetNumObjects());
  node.set_num_children(encoded.GetNumChildren() + 1);
  node.set_offset(encoded.GetOffset());
  node.set_split_value(encoded.GetSplitValue());
  return node;
}

KdNode64 KdNode64Factory::CreateLeaf(uint32_t order) const {
  return KdNode64(order, 0, 0);
}

KdNode64 KdNode64Factory::CreateInternal(uint32_t order) const {
  return KdNode64(KdNode64::kSplitX, order, 0, 0, 0.0f);
}

EncodedKdNode64 KdNode64Factory::CreateEncodedNode(const KdNode64& node) const {
  EncodedKdNode64 encoded;
  encoded.SetType(node.type());
  encoded.SetOrder(node.order());
  encoded.SetNumChildren(node.num_children() - 1);
  encoded.SetOffset(node.offset());
  if (node.IsLeaf())
    encoded.SetNumObjects(node.num_objects());
  else
    encoded.SetSplitValue(node.split_value());
  return encoded;
}
} // namespace ray

