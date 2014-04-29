/*
 * kdtree.hpp
 *
 *  Created on: Apr 22, 2014
 *      Author: agrippa
 */

#ifndef KDTREE_HPP_
#define KDTREE_HPP_
#include <stdint.h>
#include <sys/types.h>
#include "scene.hpp"
#include "shape.hpp"
#include "tree_base.hpp"
namespace ray {
template<class SceneObject, class Node, class EncodedNode, class NodeFactory>
class Kdtree: public TreeBase<SceneObject, Node, EncodedNode, NodeFactory> {
public:
  enum SplitPolicy {
    kSpatialMedian = 0, kFullSAH = 1
  };
  typedef std::vector<const SceneObject*> ObjectVector;

  Kdtree() :
      TreeBase<SceneObject, Node, EncodedNode, NodeFactory>::TreeBase(),
          split_policy_(kSpatialMedian) {
  }

  virtual ~Kdtree() {
  }

  const SplitPolicy& split_policy() const {
    return split_policy_;
  }

  void set_split_policy(const SplitPolicy& policy) {
    split_policy_ = policy;
  }
protected:
  enum SplitResult {
    kSplitX = 0, kSplitY, kSplitZ, kLeaf
  };
  typedef TreeBase<SceneObject, Node, EncodedNode, NodeFactory> TreeType;
  typedef typename TreeType::WorkNode WorkNodeType;
  typedef typename TreeType::WorkList WorkListType;

  SplitPolicy split_policy_;

  virtual BoundingBox GetChildBounds(const Node& node,
      const BoundingBox& bounds, uint32_t i) const {
    BoundingBox child_bounds = bounds;
    uint32_t dim = static_cast<uint32_t>(node.type());
    if (i == 0)
      child_bounds.max()[dim] = node.split_value();
    else
      child_bounds.min()[dim] = node.split_value();
    return child_bounds;
  }

  void EvaluateSpatialMedian(Node* parent, Node&, WorkNodeType& child_work,
      float& split_value, SplitResult& split_result) {
    //std::cout << "EvaluateSpatialMedian" << std::endl;
    uint32_t dim = static_cast<uint32_t>(split_result);
    if (parent)
      dim = (static_cast<uint32_t>(parent->type()) + 1) % 3;
    split_value = 0.5f
        * (child_work.bounds.min()[dim] + child_work.bounds.max()[dim]);
    split_result = static_cast<SplitResult>(dim);
    //std::cout << "split_result = " << split_result << std::endl;
  }

  void EvaluateFullSAH(Node*, Node&, WorkNodeType&, float&, SplitResult&) {
    //std::cout << "EvaluateFullSAH" << std::endl;
  }

  void EvaluateSplit(Node* parent, Node& child, WorkNodeType& child_work,
      float& split_value, SplitResult& split_result) {
    switch (split_policy_) {
    case kSpatialMedian:
      EvaluateSpatialMedian(parent, child, child_work, split_value,
          split_result);
      break;
    case kFullSAH:
      EvaluateFullSAH(parent, child, child_work, split_value, split_result);
      break;
    default:
      EvaluateSpatialMedian(parent, child, child_work, split_value,
          split_result);
    }
  }

  virtual void IntersectChildren2(const Node& node, const BoundingBox& bounds,
      const Ray& ray, float t_near, float t_far, Node* children,
      BoundingBox* child_bounds, uint32_t& count) const {
    uint32_t dim = static_cast<uint32_t>(node.type());
    const glm::vec3& origin = ray.origin();
    const glm::vec3& direction = ray.direction();
    float split_value = node.split_value();
    float t_split = (split_value - origin[dim]) / direction[dim];
    bool has_near = false, has_far = false;
    Node near = Node(), far = Node(), temp_node;
    BoundingBox near_bounds = BoundingBox(), far_bounds = BoundingBox(),
        temp_bounds;

    int nchild = 0;
    for (uint32_t i = 0; i < node.num_children(); ++i) {
      temp_node = this->GetIthChildOf(node, i);
      temp_bounds = this->GetChildBounds(node, bounds, temp_node.order());
      ++nchild;
      if (0 == temp_node.order()) {
        has_near = true;
        near = temp_node;
        near_bounds = temp_bounds;
      } else {
        has_far = true;
        far = temp_node;
        far_bounds = temp_bounds;
      }
    }

    if (direction[dim] < 0) {
      std::swap(has_near, has_far);
      std::swap(near, far);
      std::swap(near_bounds, far_bounds);
    }

    count = 0;
    if (has_near && t_near < t_split) {
      children[count] = near;
      child_bounds[count] = near_bounds;
      ++count;
    }
    if (has_far && t_far > t_split) {
      children[count] = far;
      child_bounds[count] = far_bounds;
      ++count;
    }

    /**Node* children2 = new Node[node.num_children()];
    BoundingBox* child_bounds2 = new BoundingBox[node.num_children()];
    uint32_t count2 = 0;
    IntersectChildren2(node, bounds, ray, t_near, t_far, children2,
        child_bounds2, count2);
    if (count > 0) {
      bool check = (count == count2) && (children2[0] == children[0])
          && (child_bounds2[0] == child_bounds[0]);
      if (!check) {
        std::cout << "\n IntersectChildren: ray = " << ray << " v = "
            << split_value << " t_near = " << t_near << " t_far = " << t_far
            << " t_ split = " << t_split << std::endl;
        std::cout << "nchild = " << nchild << " has_near = " << has_near
            << " has_far = " << has_far << std::endl;
        std::cout << "node = " << node << " bounds = " << bounds << std::endl;
        std::cout << "Actual*******" << std::endl;
        for (uint32_t i = 0; i < count; ++i) {
          float t_n = 0.0f, t_f = 0.0f;
          child_bounds[i].Intersect(ray, t_n, t_f);
          std::cout << children[i] << " " << child_bounds[i] << " t_n = " << t_n
              << " t_f = " << t_f << std::endl;
        }
        std::cout << "Expected*******" << std::endl;
        for (uint32_t i = 0; i < count2; ++i) {
          float t_n = 0.0f, t_f = 0.0f;
          child_bounds2[i].Intersect(ray, t_n, t_f);
          std::cout << children2[i] << " " << child_bounds2[i] << " t_n = "
              << t_n << " t_f = " << t_f << std::endl;
        }
        std::cout << std::endl << std::flush;
        assert(check);
      }
    }
    **/
  }

  struct SortHolder {
    float t_near;
    float t_far;
    Node child;
    BoundingBox bounds;
    SortHolder() :
        t_near(0.0f), t_far(0.0f), child(Node()), bounds(BoundingBox()) {
    }

    SortHolder(const SortHolder& h) :
        t_near(h.t_near), t_far(h.t_far), child(h.child), bounds(h.bounds) {
    }

    explicit SortHolder(float t0, float t1, const Node& n, const BoundingBox& b) :
        t_near(t0), t_far(t1), child(n), bounds(b) {
    }

    bool operator<(const SortHolder& s) const {
      return t_near < s.t_near;
    }
  };

  virtual void IntersectChildren(const Node& node, const BoundingBox& bounds,
      const Ray& ray, float, float, Node* children, BoundingBox* child_bounds,
      uint32_t& count) const {
    float t_near;
    float t_far;
    SortHolder h[2];
    count = 0;
    for (uint32_t i = 0; count < 2 && i < node.num_children(); ++i) {
      children[count] = this->GetIthChildOf(node, i);
      child_bounds[count] = this->GetChildBounds(node, bounds,
          children[count].order());
      if (child_bounds[count].Intersect(ray, t_near, t_far)) {
        h[count] = SortHolder(t_near, t_far, children[count],
            child_bounds[count]);
       // if (this->trace_)
       //   std::cout << "i = " << i << " t_near = " << t_near << " t_far = "
       //       << t_far << " child = " << children[count] << "\n";
        ++count;
      }
    }
    if (this->trace_)
      std::cout << "Sort children:\n";
    std::sort(&h[0], &h[0] + count);
    for (uint32_t i = 0; i < count; ++i) {
      children[i] = h[i].child;
      child_bounds[i] = h[i].bounds;
      if (this->trace_)
        std::cout << "i = " << i << " t_near = " << h[i].t_near << " t_far = "
            << h[i].t_far << " node = " << children[i] << "\n";
    }
  }

  virtual void BuildRoot(Node& root, WorkNodeType& work_root) {
    float split_value = 0.0f;
    SplitResult split_result = kSplitZ;
    EvaluateSplit(NULL, root, work_root, split_value, split_result);
    if (work_root.objects.size() <= this->max_leaf_size_
        || 0 == this->max_depth_ || kLeaf == split_result)
      root = this->GetNodeFactory().CreateLeaf(0);
    else {
      root = this->GetNodeFactory().CreateInternal(0);
      root.set_type(static_cast<uint32_t>(split_result));
      root.set_split_value(split_value);
    }
  }

  virtual void BuildLeaf(Node& node, WorkNodeType& work_node) {
    ++this->num_leaves_;
    node.set_offset(this->scene_objects_.size());
    node.set_num_objects(work_node.objects.size());
    while (!work_node.objects.empty()) {
      this->scene_objects_.push_back(work_node.objects.back());
      work_node.objects.pop_back();
    }
  }

  virtual void BuildInternal(Node& node, WorkNodeType& work_node,
      WorkListType& next_list, uint32_t depth) {
    //std::cout << "BuildInternal: start_num_children = " << node.num_children() << std::endl;
    ++this->num_internal_nodes_;
    WorkNodeType child_work_nodes[2]; // process children tentatively
    node.set_offset(this->nodes_.size()); // children will have nodes pushed
    for (uint32_t j = 0; j < 2; ++j)
      child_work_nodes[j] = WorkNodeType( // initialize child lists
          this->GetChildBounds(node, work_node.bounds, j));
    while (!work_node.objects.empty()) {
      const SceneObject* obj = work_node.objects.back();
      work_node.objects.pop_back();
      for (uint32_t j = 0; j < 2; ++j)  // distribute to children
        if (obj->GetBounds().Overlap(child_work_nodes[j].bounds))
          child_work_nodes[j].objects.push_back(obj);
    }
    int num_children = 0;
    for (int j = 1; j >= 0; --j) {
      // If a child has a non-empty object list, process it.
      if (child_work_nodes[j].objects.size() > 0) {
        ++num_children;
        node.set_num_children(node.num_children() + 1); // update parent size
        uint32_t count = child_work_nodes[j].objects.size();
        Node child;
        float split_value = 0.0f;
        SplitResult split_result = kSplitX;
        EvaluateSplit(&node, child, child_work_nodes[j], split_value,
            split_result);
        if (depth + 1 >= this->max_depth_ || count <= this->max_leaf_size_
            || kLeaf == split_result)
          child = this->GetNodeFactory().CreateLeaf(j);
        else {
          child = this->GetNodeFactory().CreateInternal(j);
          child.set_type(static_cast<uint32_t>(split_result));
          child.set_split_value(split_value);
        }
        child_work_nodes[j].node_index = this->nodes_.size();
        next_list.push_back(child_work_nodes[j]);
        this->nodes_.push_back(this->EncodeNode(child));
      }
    }
    node.set_num_children(num_children);
    //std::cout << "BuildInternal: end_num_children = " << node.num_children() << std::endl;
  }
};
} // namespace ray
#endif /* KDTREE_HPP_ */
