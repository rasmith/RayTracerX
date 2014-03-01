/*
 * octree_base.cpp
 *
 *  Created on: Feb 5, 2014
 *      Author: agrippa
 */
#include <sys/types.h>
#include "io_utils.hpp"
#include "octree_base.hpp"
#include "scene.hpp"
#include "shape.hpp"
namespace ray {
const uint32_t OctreeBase::kMaxDepth = 3;
const uint32_t OctreeBase::kMaxLeafSize = 32;
Accelerator::~Accelerator() {
}

Accelerator::Accelerator() :
    SceneShape(), trace_(false) {
}

bool Accelerator::trace() const {
  return trace_;
}

void Accelerator::set_trace(bool trace) {
  trace_ = trace;
}

OctreeBase::OctreeBase() :
    Accelerator() {
}

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
    if ((octant >> i) & 0x1)
      child_bounds.min()[i] = center[i];
    else
      child_bounds.max()[i] = center[i];
  }
  return child_bounds;
}

void OctreeBase::PrintNode(std::ostream& out, const OctNode& node,
    const BoundingBox& bbox, int depth) const {
  for (int i = 0; i < depth; ++i)
    out << " ";
  out << node << " " << bbox << "\n";
}

void OctreeBase::Print(std::ostream& out) const {
  std::vector<OctNode> nodes;
  std::vector<int> depths;
  std::vector<BoundingBox> bounds;
  nodes.clear();
  depths.clear();
  bounds.clear();
  nodes.push_back(GetRoot());
  bounds.push_back(GetBounds());
  depths.push_back(0);
  while (!nodes.empty()) {
    OctNode node = nodes.back();
    int depth = depths.back();
    BoundingBox bbox = bounds.back();
    nodes.pop_back();
    bounds.pop_back();
    depths.pop_back();
    PrintNode(out, node, bbox, depth);
    if (node.IsInternal()) {
      for (uint32_t i = 0; i < node.size(); ++i) {
        OctNode child = GetIthChildOf(node, i);
        nodes.push_back(child);
        bounds.push_back(GetChildBounds(bbox, child.octant()));
        depths.push_back(depth + 1);
      }
    }
  }
}

OctNodeFactory& OctreeBase::GetNodeFactory() const {
  return OctNodeFactory::GetInstance();
}
bool OctreeBase::Intersect(const Ray& ray, Isect& isect) const {
  //std::cout << "OctreeBase::Intersect: ray = " << ray << "\n";
  return Traverse(GetRoot(), GetBounds(), ray, isect, 0);
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
struct SortHolder {
  float t_near;
  float t_far;
  OctNode child;
  BoundingBox bounds;
  SortHolder() :
      t_near(0.0f), t_far(0.0f), child(OctNode()), bounds(BoundingBox()) {
  }
  SortHolder(const SortHolder& h) :
      t_near(h.t_near), t_far(h.t_far), child(h.child), bounds(h.bounds) {
  }
  explicit SortHolder(float t0, float t1, const OctNode& n,
      const BoundingBox& b) :
      t_near(t0), t_far(t1), child(n), bounds(b) {
  }
  bool operator<(const SortHolder& s) const {
    return t_near < s.t_near;
  }
};
void OctreeBase::IntersectChildren(const OctNode& node,
    const BoundingBox& bounds, const Ray& ray, OctNode* children,
    BoundingBox* child_bounds, uint32_t& count) const {
  float t_near;
  float t_far;
  SortHolder h[4];
  count = 0;
  for (uint32_t i = 0; count < 4 && i < node.size(); ++i) {
    children[count] = GetIthChildOf(node, i);
    child_bounds[count] = GetChildBounds(bounds, children[count].octant());
    if (child_bounds[count].Intersect(ray, t_near, t_far)) {
      h[count] = SortHolder(t_near, t_far, children[count],
          child_bounds[count]);
      if (trace_)
        std::cout << "i = " << i << " t_near = " << t_near << " t_far = "
            << t_far << " child = " << children[count] << "\n";
      ++count;
    }
  }
  if (trace_)
    std::cout << "Sort children:\n";
  std::sort(&h[0], &h[0] + count);
  for (uint32_t i = 0; i < count; ++i) {
    children[i] = h[i].child;
    child_bounds[i] = h[i].bounds;
    if (trace_)
      std::cout << "i = " << i << " t_near = " << h[i].t_near << " t_far = "
          << h[i].t_far << " node = " << children[i] << "\n";
  }
}
bool OctreeBase::TraverseStackless(const OctNode& root,
    const BoundingBox& bounds, const Ray& ray, Isect& isect) const {
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
    if (current.IsLeaf())
      hit = IntersectLeaf(current, ray, isect);
    else {
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
    const Ray& ray, Isect& isect, uint32_t depth) const {
  //for (uint32_t i = 0; i < depth; ++i)
  //  std::cout << " ";
  //std::cout << node << "\n";
  if (depth > kMaxDepth) // check depth first
    return false;
  float t_near, t_far;
  if (!bounds.Intersect(ray, t_near, t_far)) // check bounds next
    return false;
  if (node.IsLeaf()) // is this a leaf?
    return IntersectLeaf(node, ray, isect);
  OctNode children[4];    // can hit at most four children
  BoundingBox child_bounds[4];
  uint32_t count = 0;
  IntersectChildren(node, bounds, ray, &children[0], &child_bounds[0], count);
  bool hit = false;
  for (uint32_t i = 0; i < count && !hit; ++i)
    hit = Traverse(children[i], child_bounds[i], ray, isect, depth + 1);
  return hit;
}

EncodedNode::EncodedNode() {
  for (int i = 0; i < 8; ++i)
    data[i] = 0;
}

EncodedNode::EncodedNode(const EncodedNode& node) {
  for (int i = 0; i < 8; ++i)
    data[i] = node.data[i];
}

EncodedNode& EncodedNode::operator =(const EncodedNode& node) {
  if (this == &node)
    return *this;
  for (int i = 0; i < 8; ++i)
    data[i] = node.data[i];
  return *this;
}

OctNode::NodeType EncodedNode::GetType() const {
  return static_cast<OctNode::NodeType>((data[0] & 0x80) >> 7);
}
uint32_t EncodedNode::GetOctant() const {
  return static_cast<uint32_t>((data[0] & 0x70) >> 4);
}
uint32_t EncodedNode::GetSize() const {
  uint32_t size = 0x0;
  uint32_t byte = 0x0;
  for (uint32_t i = 5; i < 8; ++i) {
    byte = static_cast<uint32_t>(data[i]);
    size = size | (byte << ((i - 5) * 8));
  }
  return size;
}
uint32_t EncodedNode::GetOffset() const {
  uint32_t offset = 0x0;
  uint32_t byte = 0x0;
  for (uint32_t i = 1; i < 5; ++i) {
    byte = static_cast<uint32_t>(data[i]);
    offset = offset | (byte << ((i - 1) * 8));
  }
  return offset;
}
void EncodedNode::SetType(OctNode::NodeType type) {
  u_char mask = static_cast<u_char>(type);
  mask = (mask << 7) & 0x80;
  data[0] = (0x7F & data[0]) | mask;
}
void EncodedNode::SetOctant(uint32_t octant) {
  u_char mask = static_cast<u_char>(octant);
  mask = (mask << 4);
  data[0] = (0x8F & data[0]) | mask;
}

void EncodedNode::SetSize(uint32_t size) {
  uint32_t byte = 0x0;
  uint32_t shift = 0;
  for (uint32_t i = 5; i < 8; ++i) {
    shift = (i - 5) * 8;
    byte = (size >> shift) & 0x000000FF;
    data[i] = static_cast<u_char>(byte);
  }
}

void EncodedNode::SetOffset(uint32_t offset) {
  uint32_t byte = 0x0;
  uint32_t shift = 0;
  for (uint32_t i = 1; i < 5; ++i) {
    shift = (i - 1) * 8;
    byte = (offset >> shift) & 0x000000FF;
    data[i] = static_cast<u_char>(byte);
  }
}
std::ostream& operator<<(std::ostream& out, const EncodedNode& node) {
  for (uint32_t i = 0; i < 8; ++i) {
    for (uint32_t j = 0; j < 8; ++j)
      out << ((0x1 << (7 - j)) & node.data[i] ? '1' : '0');
    out << ' ';
  }
  return out;
}

OctNode::OctNode() :
    type_(kInternal), octant_(0), size_(0), offset_(0) {
}

OctNode::OctNode(const OctNode::NodeType& node_type, uint32_t node_octant,
    uint32_t node_size, uint32_t node_offset) :
    type_(node_type), octant_(node_octant), size_(node_size),
        offset_(node_offset) {
}

OctNode::OctNode(const OctNode& node) :
    type_(node.type_), octant_(node.octant_), size_(node.size_),
        offset_(node.offset_) {
}

bool OctNode::operator==(const OctNode& node) const {
  return type() == node.type() && octant() == node.octant()
      && size() == node.size() && offset() == node.offset();
}

OctNode& OctNode::operator=(const OctNode& node) {
  if (this == &node)
    return *this;
  type_ = node.type_;
  octant_ = node.octant_;
  size_ = node.size_;
  offset_ = node.offset_;
  return *this;
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
  node.set_size(encoded.GetSize());
  node.set_octant(encoded.GetOctant());
  node.set_type(encoded.GetType());
  node.set_offset(encoded.GetOffset());
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
  encoded.SetType(node.type());
  encoded.SetOctant(node.octant());
  encoded.SetOffset(node.offset());
  encoded.SetSize(node.size());
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
  return type_ == kLeaf;
}
bool OctNode::IsInternal() const {
  return type_ == kInternal;
}
std::ostream& operator<<(std::ostream& out, const OctNode& node) {
  if (node.IsLeaf())
    out << "L:";
  else
    out << "N:";
  out << "[" << node.octant() << "] #" << node.size() << " @" << node.offset();
  return out;
}
}  // namespace ray
