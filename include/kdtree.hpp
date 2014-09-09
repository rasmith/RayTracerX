/*
 * kdtree.hpp
 *
 *  Created on: Apr 22, 2014
 *      Author: agrippa
 */

#ifndef KDTREE_HPP_
#define KDTREE_HPP_

#include <algorithm>
#include <deque>
#include <stdint.h>
#include <sys/types.h>

#include "scene.hpp"
#include "shape.hpp"
#include "tree_base.hpp"

namespace ray {
template <class SceneObject, class Node, class EncodedNode, class NodeFactory>
class Kdtree : public TreeBase<SceneObject, Node, EncodedNode, NodeFactory> {
 public:
  enum SplitPolicy {
    kSpatialMedian = 0,
    kFullSAH = 1
  };
  typedef std::vector<const SceneObject*> ObjectVector;

  Kdtree()
      : TreeBase<SceneObject, Node, EncodedNode, NodeFactory>::TreeBase(),
        split_policy_(kSpatialMedian) {}

  virtual ~Kdtree() {}

  const SplitPolicy& split_policy() const { return split_policy_; }

  void set_split_policy(const SplitPolicy& policy) { split_policy_ = policy; }

 protected:
  enum SplitResult {
    kSplitX = 0,
    kSplitY,
    kSplitZ,
    kLeaf
  };
  enum PlanarSplitSide {
    kPlanarLeft,
    kPlanarRight
  };
  typedef TreeBase<SceneObject, Node, EncodedNode, NodeFactory> TreeType;
  typedef typename TreeType::WorkNode WorkNodeType;
  typedef typename TreeType::WorkList WorkListType;

  struct Event {
    enum EventType {
      kEnd = 0,
      kPlanar = 1,
      kStart = 2
    };
    Event() : value(0.0f), type(kStart), obj(NULL), id(GetNewId()) {}
    Event(const Event& e)
        : value(e.value), type(e.type), obj(e.obj), id(e.id) {}
    Event(float v, EventType t, const SceneObject* o)
        : value(v), type(t), obj(o), id(GetNewId()) {}
    void operator=(const Event& e) { Copy(e); }
    void Copy(const Event& e) {
      value = e.value;
      type = e.type;
      obj = e.obj;
      id = e.id;
      assert(!(*this < e) && !(e < *this));
    }
    bool operator<(const Event& e) const {
      assert(obj != NULL);
      if (this == &e) return false;
      if (value < e.value) return true;
      if (value > e.value) return false;
      if (type < e.type) return true;
      if (type > e.type) return false;
      if (id < e.id) return true;
      return false;
    }
    uint32_t GetNewId() {
      static uint32_t next = 0;
      return ++next;
    }
    float value;
    EventType type;
    const SceneObject* obj;
    uint32_t id;
  };

  // typedef std::list<Event> EventList;
  typedef std::deque<Event> EventList;
  typedef std::vector<Event> EventVector;

  struct SahWorkInfo {
    SahWorkInfo() : split_side(kPlanarLeft) {
      for (int i = 0; i < 3; ++i) events[i].clear();
    }
    ~SahWorkInfo() {
      for (int i = 0; i < 3; ++i) events[i].clear();
    }
    EventList events[3];
    PlanarSplitSide split_side;
  };

  SplitPolicy split_policy_;

  void CreateEvents(const ObjectVector& objects, SahWorkInfo& work_info) {
    // if (this->trace_) {
    // std::cout << "EvaluateFullSAH:begin sorting\n";
    //}
    BoundingBox bounds;
    const SceneObject* obj = NULL;
    EventList event_list;
    for (uint32_t d = 0; d < 3; ++d) {
      for (uint32_t i = 0; i < objects.size(); ++i) {
        obj = objects[i];
        bounds = obj->GetBounds();
        if (bounds.max()[d] == bounds.min()[d]) {
          work_info.events[d]
              .push_back(Event(bounds.min()[d], Event::kPlanar, obj));
        } else {
          work_info.events[d]
              .push_back(Event(bounds.min()[d], Event::kStart, obj));
          work_info.events[d]
              .push_back(Event(bounds.max()[d], Event::kEnd, obj));
        }
      }
      std::sort(work_info.events[d].begin(), work_info.events[d].end());
    }
    // if (this->trace_) {
    // std::cout << "EvaluateFullSAH:end sorting\n";
    // }
  }

  void DistributeEvents(float value, uint32_t split_dim, uint32_t list_dim,
                        SahWorkInfo* parent_info, SahWorkInfo* left_info,
                        SahWorkInfo* right_info) {
    EventList* parent_list = &parent_info->events[list_dim];
    EventList* left_list = (left_info ? &left_info->events[list_dim] : NULL);
    EventList* right_list = (right_info ? &right_info->events[list_dim] : NULL);
    Event e = Event();
    BoundingBox b = BoundingBox();
    bool is_left = (parent_info->split_side == kPlanarLeft);
    while (!parent_list->empty()) {
      e = parent_list->front();
      parent_list->pop_front();
      b = e.obj->GetBounds();
      bool planar_left =
          (e.type == Event::kPlanar && e.value == value && is_left);
      bool planar_right =
          (e.type == Event::kPlanar && e.value == value && !is_left);
      if (left_list && (planar_left || b.min()[split_dim] <= value))
        left_list->push_back(e);
      if (right_list && (planar_right || b.max()[split_dim] >= value))
        right_list->push_back(e);
    }
  }

  virtual void ProcessWorkInfo(const Node& node, WorkNodeType& work_node,
                               WorkNodeType* child_work_nodes) {
    if (kFullSAH != split_policy_) return;
    SahWorkInfo* parent_info =
        (work_node.work_info
             ? reinterpret_cast<SahWorkInfo*>(work_node.work_info)
             : NULL);
    if (child_work_nodes && parent_info) {
      float value = node.split_value();
      uint32_t split_dim = static_cast<uint32_t>(node.type());
      WorkNodeType& left = child_work_nodes[0];
      WorkNodeType& right = child_work_nodes[1];

      SahWorkInfo* left_info =
          (left.objects.size() > 0 ? new SahWorkInfo : NULL);
      SahWorkInfo* right_info =
          (right.objects.size() > 0 ? new SahWorkInfo : NULL);

      for (int d = 0; d < 3; ++d)
        DistributeEvents(value, split_dim, d, parent_info, left_info,
                         right_info);

      left.work_info = reinterpret_cast<void*>(left_info);
      right.work_info = reinterpret_cast<void*>(right_info);
    }
    if (parent_info) {
      delete parent_info;
      work_node.work_info = NULL;
    }
    if (child_work_nodes)
      for (uint32_t j = 0; j < 2; ++j)
        assert((child_work_nodes[j].objects.size() > 0 &&
                child_work_nodes[j].work_info != NULL) ||
               (child_work_nodes[j].objects.size() == 0 &&
                child_work_nodes[j].work_info == NULL));
  }

  virtual BoundingBox GetChildBounds(const Node& node,
                                     const BoundingBox& bounds,
                                     uint32_t i) const {
    BoundingBox child_bounds = bounds;
    uint32_t dim = static_cast<uint32_t>(node.type());
    if (i == 0)
      child_bounds.max()[dim] = node.split_value();
    else
      child_bounds.min()[dim] = node.split_value();
    return child_bounds;
  }

  float GetLeafCost(int num_objects) const {
    return static_cast<float>(num_objects);
  }

  void EvaluateSpatialMedian(Node* parent, Node&, WorkNodeType& child_work,
                             float& split_value, SplitResult& split_result) {
    uint32_t dim = static_cast<uint32_t>(split_result);
    if (parent) dim = (static_cast<uint32_t>(parent->type()) + 1) % 3;
    split_value =
        0.5f * (child_work.bounds.min()[dim] + child_work.bounds.max()[dim]);
    split_result = static_cast<SplitResult>(dim);
  }

  float ComputeSAH(int left_count, int right_count, float left_area,
                   float right_area, float total_area) const {
    float cost_traverse = 1.0f;
    float cost_intersect = 1.0f;
    return cost_traverse +
           cost_intersect *
               (left_area * left_count + right_area * right_count) / total_area;
  }

  void UpdateCounts(const Event& event, int& right_count, int& left_count,
                    int& planar_count) const {
    switch (event.type) {
      case Event::kEnd:
        --right_count;
        break;
      case Event::kPlanar:
        ++planar_count;
        break;
      case Event::kStart:
        ++left_count;
        break;
      default:
        break;
    }
  }

  void UpdateCost(float current_value, float min_val, float max_val, float area,
                  float sum_other, float prod_other, int left_count,
                  int right_count, int planar_count, float& best_cost,
                  float& best_value, PlanarSplitSide& best_side) const {
    float area_left = (current_value - min_val) * sum_other + prod_other;
    float area_right = (max_val - current_value) * sum_other + prod_other;
    float cost_left = ComputeSAH(left_count + planar_count, right_count,
                                 area_left, area_right, area);
    float cost_right = ComputeSAH(left_count, right_count + planar_count,
                                  area_left, area_right, area);
    float cost = std::min(cost_left, cost_right);
    PlanarSplitSide side =
        (cost_left < cost_right ? kPlanarLeft : kPlanarRight);
    if (cost < best_cost) {
      best_cost = cost;
      best_side = side;
      best_value = current_value;
    }
  }

  // FindBestPlaneInList find the best plane in the "events" list.
  // Since the number of events will probably differ from the number of
  // objects, the number of objects needs to be passed in as "num_objects".
  // Since this function does not take a dimension as an argument,
  // it asks for the minimum and maximum values of the respective
  // dimension and these are to be passed in as "min_val" and "max_val",
  // respectively. To make the necessary calculations, the extents of the other
  // dimensions are needed,  which are passed in as "extent0" and "extent1".
  // The area of the surrounding bounding box needs to be passed in as "area".
  // Finally, the best cost, best split value, and corresponding planar
  // split side will be returned into "best_cost", "best_value", and
  // "best_side", respectively.
  void FindBestPlaneInList(const EventList& events, float extent0,
                           float extent1, float min_val, float max_val,
                           int num_objects, float area, float& best_cost,
                           float& best_value,
                           PlanarSplitSide& best_side) const {
    const float sum_other = 2.0f * (extent0 + extent1);
    const float prod_other = 2.0f * (extent0 * extent1);
    int left_count = 0, right_count = num_objects, planar_count = 0;
    best_cost = std::numeric_limits<float>::max();
    best_value = std::numeric_limits<float>::max();
    for (uint32_t i = 0; i < events.size(); ++i) {
      float current_value = glm::clamp(events[i].value, min_val, max_val);
      UpdateCounts(events[i], left_count, right_count, planar_count);
      uint32_t next = i + 1;
      bool update_cost =
          (next == events.size() ||
           current_value != glm::clamp(events[next].value, min_val, max_val));
      if (update_cost)
        UpdateCost(current_value, min_val, max_val, area, sum_other, prod_other,
                   left_count, right_count, planar_count, best_cost, best_value,
                   best_side);
    }
  }

  void EvaluateFullSAH(Node* parent, Node&, WorkNodeType& work_node,
                       float& value, SplitResult& split_result) {
    SahWorkInfo* info = NULL;
    PlanarSplitSide current_side = kPlanarLeft;
    float best_cost = std::numeric_limits<float>::max(), current_cost = 0.0f;
    float current_value = 0.0f;
    float total_area = work_node.bounds.GetArea();
    BoundingBox bounds = work_node.bounds;
    if (!parent) {
      info = new SahWorkInfo;
      CreateEvents(work_node.objects, *info);
      work_node.work_info = reinterpret_cast<void*>(info);
    } else
      info = reinterpret_cast<SahWorkInfo*>(work_node.work_info);
    assert(info != NULL);
    glm::vec3 extents = bounds.max() - bounds.min();
    const uint32_t kOtherIndices[3][2] = {{1, 2}, {0, 2}, {0, 1}};
    for (uint32_t d = 0; d < 3; ++d) {
      FindBestPlaneInList(info->events[d], extents[kOtherIndices[d][0]],
                          extents[kOtherIndices[d][1]], bounds.min()[d],
                          bounds.max()[d], work_node.objects.size(), total_area,
                          current_cost, current_value, current_side);
      if (current_cost < best_cost) {
        best_cost = current_cost;
        split_result = static_cast<SplitResult>(d);
        value = current_value;
        // Squirrel away the planar split side here.
        info->split_side = current_side;
      }
    }
    if (best_cost > GetLeafCost(work_node.objects.size())) split_result = kLeaf;
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
                                  const Ray& ray, float t_near, float t_far,
                                  Node* children, BoundingBox* child_bounds,
                                  uint32_t& count) const {
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
    SortHolder()
        : t_near(0.0f), t_far(0.0f), child(Node()), bounds(BoundingBox()) {}

    SortHolder(const SortHolder& h)
        : t_near(h.t_near), t_far(h.t_far), child(h.child), bounds(h.bounds) {}

    explicit SortHolder(float t0, float t1, const Node& n, const BoundingBox& b)
        : t_near(t0), t_far(t1), child(n), bounds(b) {}

    bool operator<(const SortHolder& s) const { return t_near < s.t_near; }
  };

  virtual void IntersectChildren(const Node& node, const BoundingBox& bounds,
                                 const Ray& ray, float, float, Node* children,
                                 BoundingBox* child_bounds,
                                 uint32_t& count) const {
    float t_near;
    float t_far;
    SortHolder h[2];
    count = 0;
    for (uint32_t i = 0; count < 2 && i < node.num_children(); ++i) {
      children[count] = this->GetIthChildOf(node, i);
      child_bounds[count] =
          this->GetChildBounds(node, bounds, children[count].order());
      if (child_bounds[count].Intersect(ray, t_near, t_far)) {
        h[count] =
            SortHolder(t_near, t_far, children[count], child_bounds[count]);
        // if (this->trace_)
        //   std::cout << "i = " << i << " t_near = " << t_near << " t_far = "
        //       << t_far << " child = " << children[count] << "\n";
        ++count;
      }
    }
    if (this->trace_) std::cout << "Sort children:\n";
    std::sort(&h[0], &h[0] + count);
    for (uint32_t i = 0; i < count; ++i) {
      children[i] = h[i].child;
      child_bounds[i] = h[i].bounds;
      if (this->trace_)
        std::cout << "i = " << i << " t_near = " << h[i].t_near
                  << " t_far = " << h[i].t_far << " node = " << children[i]
                  << "\n";
    }
  }

  virtual void BuildRoot(Node& root, WorkNodeType& work_root) {
    float split_value = 0.0f;
    SplitResult split_result = kSplitX;
    EvaluateSplit(NULL, root, work_root, split_value, split_result);
    if (work_root.objects.size() <= this->max_leaf_size_ ||
        0 == this->max_depth_ || kLeaf == split_result)
      root = this->GetNodeFactory().CreateLeaf(0);
    else {
      root = this->GetNodeFactory().CreateInternal(0);
      root.set_type(static_cast<uint32_t>(split_result));
      root.set_split_value(split_value);
    }
  }

  virtual void BuildLeaf(Node& node, WorkNodeType& work_node) {
    ++this->num_leaves_;
    ProcessWorkInfo(node, work_node, NULL);
    node.set_offset(this->scene_objects_.size());
    node.set_num_objects(work_node.objects.size());
    while (!work_node.objects.empty()) {
      this->scene_objects_.push_back(work_node.objects.back());
      work_node.objects.pop_back();
    }
  }

  virtual void BuildInternal(Node& node, WorkNodeType& work_node,
                             WorkListType& next_list, uint32_t depth) {
    // std::cout << "BuildInternal: start_num_children = " <<
    // node.num_children() << std::endl;
    ++this->num_internal_nodes_;
    WorkNodeType child_work_nodes[2];      // process children tentatively
    node.set_offset(this->nodes_.size());  // children will have nodes pushed
    for (uint32_t j = 0; j < 2; ++j)
      child_work_nodes[j] = WorkNodeType(  // initialize child lists
          this->GetChildBounds(node, work_node.bounds, j));
    while (!work_node.objects.empty()) {
      const SceneObject* obj = work_node.objects.back();
      work_node.objects.pop_back();
      for (uint32_t j = 0; j < 2; ++j)  // distribute to children
        if (obj->GetBounds().Overlap(child_work_nodes[j].bounds))
          child_work_nodes[j].objects.push_back(obj);
    }
    ProcessWorkInfo(node, work_node, &child_work_nodes[0]);
    int num_children = 0;
    for (int j = 1; j >= 0; --j) {
      // If a child has a non-empty object list, process it.
      if (child_work_nodes[j].objects.size() > 0) {
        ++num_children;
        node.set_num_children(node.num_children() + 1);  // update parent size
        uint32_t count = child_work_nodes[j].objects.size();
        Node child;
        float split_value = 0.0f;
        SplitResult split_result = kSplitX;
        EvaluateSplit(&node, child, child_work_nodes[j], split_value,
                      split_result);
        if (depth + 1 >= this->max_depth_ || count <= this->max_leaf_size_ ||
            kLeaf == split_result)
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
    // std::cout << "BuildInternal: end_num_children = " << node.num_children()
    // << std::endl;
  }
};
}  // namespace ray
#endif /* KDTREE_HPP_ */
