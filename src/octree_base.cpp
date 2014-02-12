/*
 * octree_base.cpp
 *
 *  Created on: Feb 5, 2014
 *      Author: agrippa
 */
#include "octree_base.hpp"
namespace ray {
const int OctreeBase::kMaxDepth = 20;
const int OctreeBase::kMaxLeafSize = 16;
uint32_t OctreeBase::PointToOctant(const BoundingBox& bounds,
    const glm::vec3& point) const {
  glm::vec3 center = bounds.GetCenter();
  uint32_t x_bit = (point[0] > center[0]);
  uint32_t y_bit = (point[1] > center[1]);
  uint32_t z_bit = (point[2] > center[2]);
  uint32_t octant = x_bit | (y_bit << 1) | (z_bit << 2);
  return octant;
}
BoundingBox OctreeBase::GetChildBounds(const BoundingBox& bounds,
    uint32_t octant) const {
  glm::vec3 center = bounds.GetCenter();
  BoundingBox child_bounds = bounds;
  for (uint32_t i = 0; i < 3; ++i) {
    if ((octant >> (i - 1)) & 0x1) {
      child_bounds.min[i] = center[i];
    } else {
      child_bounds.max[i] = center[i];
    }
  }
  return child_bounds;
}
void PrintNode(std::ostream& out, const OctNode& node, int depth) const {
  for (int i = 0; i < depth; ++i)
    out << " ";
  if (node.IsLeaf())
    out << "L:";
  else
    out << "N:";
  out << "[" << node.octant() << "] #" << node.size() << " @" << node.offset()
      << "\n";
}
void OctreeBase::Print(std::ostream& out) const {
  std::vector<OctNode> nodes;
  std::vector<int> depths;
  nodes.clear();
  nodes.push_back(GetRoot());
  depths.push_back(0);
  while (!nodes.empty()) {
    OctNode node = nodes.back();
    int depth = depths.back();
    nodes.pop_back();
    depths.pop_back();
    PrintNode(out, node, depth);
    for (int i = 0; i < node.size(); ++i) {
      nodes.push_back(GetIthChildOf(node, i));
      depths.push_back(depth + 1);
    }
  }
}
OctNodeFactory& OctreeBase::GetNodeFactory() const {
  return OctNodeFactory::GetInstance();
}
bool OctreeBase::Intersect(const Ray& ray, Isect& isect) const {
  return TraverseStackless(GetRoot(), GetBounds(), ray, isect, 0);
}
///////
//
// IntersectChildren
//
//  Intersect children of given node and return in sorted order
//  that the given ray hit them along with child bounding boxes.
//  Can hit at most four (4) children.  Arrays passed in are
//  assumed to be of length four (4).
//
//////
void OctreeBase::IntersectChildren(const OctNode& node,
    const BoundingBox& bounds, const Ray& ray, OctNode* children,
    BoundingBox* child_bounds, uint32_t& count) const {
  float t_near_vals[4];
  float t_far_vals[4];
  count = 0;
  for (int i = 0; count < 4 && i < node.size(); ++i) {
    children[count] = GetIthChildOf(node, i);
    child_bounds[count] = GetChildBounds(bounds, children[count].octant());
    if (child_bounds[count].Intersect(ray, t_near_vals[count],
        t_far_vals[count]))
      ++count;
  }
  // sort by t_near - selection sort
  for (int i = 0; i < count; ++i) {
    int *pos = std::min_element(t_near_vals + i, t_near_vals + count);
    int k = pos - &count[0];
    std::swap(children[i], children[k]);
    std::swap(child_bounds[i], child_bounds[k]);
    std::swap(t_near_vals[i], t_far_vals[k]);
    std::swap(t_far_vals[i], t_far_vals[k]);
  }
}
bool OctreeBase::TraverseStackless(const OctNode& root,
    const BoundingBox& bounds, const Ray& ray, Isect& isect, int depth) const {
  float t_near, t_far;
  if (!bounds.Intersect(ray, t_near, t_far))
    return false;
  std::vector<OctNode> node_stack;
  std::vector<BoundingBox> bounds_stack;
  bool hit = false;
  uint32_t count = 0;
  OctNode current, child;
  OctNode children[4];    // can hit at most four children
  BoundingBox child_bounds[4];
  BoundingBox current_bounds;
  node_stack.clear();
  bounds_stack.clear();
  node_stack.push_back(root);
  bounds_stack.push_back(bounds);
  while (!node_stack.empty() && !hit) {
    current = node_stack.back();
    node_stack.pop_back();
    current_bounds = bounds_stack.back();
    bounds_stack.pop_back();
    if (current.IsLeaf()) {
      hit = IntersectLeaf(current, current_bounds, ray, isect);
    } else {
      IntersectChildren(current, bounds, ray, &children[0], &child_bounds[0],
          count);
      for (uint32_t i = 0; i < count; ++i) {
        node_stack.push_back(children[count - i - 1]);
        bounds_stack.push_back(child_bounds[count - i - 1]);
      }
    }
  }
  return hit;
}
bool OctreeBase::Traverse(const OctNode& node, const BoundingBox& bounds,
    const Ray& ray, Isect& isect, int depth) const {
  if (depth > kMaxDepth) // check depth first
    return false;
  float t_near, t_far;
  if (!bounds.Intersect(ray, t_near, t_far)) // check bounds next
    return false;
  if (node.IsLeaf()) // is this a leaf?
    return IntersectLeaf(node, bounds, ray, isect);
  OctNode children[4];    // can hit at most four children
  BoundingBox child_bounds[4];
  uint32_t count = 0;
  IntersectChildren(node, bounds, ray, &children[0], &child_bounds[0], count);
  bool hit = false;
  for (int i = 0; i < count && !hit; ++i)
    hit = Traverse(children[i], child_bounds[i], ray, isect, depth + 1);
  return hit;
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
OctNodeFactory & OctNodeFactory::GetInstance() {
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
