/*
 * octree.hpp
 *
 *  Created on: Dec 19, 2013
 *      Author: agrippa
 */

#ifndef SAH_OCTREE_HPP_
#define SAH_OCTREE_HPP_
#include <sys/types.h>
#include <stdint.h>
#include <algorithm>
#include <cstring>
#include <limits>
#include <list>
#include <vector>
#include "grid.hpp"
#include "octree.hpp"
#include "octree_base.hpp"
#include "sah_octnode.hpp"
#include "shape.hpp"
namespace ray {
float K_I = 1.0f;
float K_T = 8.0f;
static bool test2 = true;
static bool test = true;
template<class SceneObject, int max_leaf_size = 16, int max_depth = 10>
class SAHOctree: public Octree<SceneObject, SAHOctNode, SAHEncodedNode,
    SAHOctNodeFactory, max_leaf_size, max_depth> {
public:
  typedef std::vector<const SceneObject*> ObjectVector;

  SAHOctree() :
          Octree<SceneObject, SAHOctNode, SAHEncodedNode, SAHOctNodeFactory,
              max_leaf_size, max_depth>::Octree() {
  }

  virtual ~SAHOctree() {
  }

  virtual uint32_t PointToOctant(const glm::vec3 &split,
      const glm::vec3& point) const {
    uint32_t x_bit = (point[0] > split[0]);
    uint32_t y_bit = (point[1] > split[1]);
    uint32_t z_bit = (point[2] > split[2]);
    uint32_t octant = x_bit | (y_bit << 1) | (z_bit << 2);
    return octant;
  }

  virtual uint32_t PointToOctant(const SAHOctNode& node, const BoundingBox&,
      const glm::vec3& point) const {
    return PointToOctant(node.point(), point);
  }

  virtual BoundingBox GetOctantBounds(const glm::vec3& point,
      const BoundingBox& bounds, uint32_t octant) const {
    BoundingBox octant_bounds = bounds;
    for (uint32_t i = 0; i < 3; ++i) {
      if ((octant >> i) & 0x1)
        octant_bounds.min()[i] = point[i];
      else
        octant_bounds.max()[i] = point[i];
    }
    return octant_bounds;
  }

  virtual BoundingBox GetChildBounds(const SAHOctNode& node,
      const BoundingBox& bounds, uint32_t octant) const {
    glm::vec3 split = node.point();
    BoundingBox child_bounds = bounds;
    for (uint32_t i = 0; i < 3; ++i) {
      if ((octant >> i) & 0x1)
        child_bounds.min()[i] = split[i];
      else
        child_bounds.max()[i] = split[i];
    }
    return child_bounds;
  }

  virtual glm::ivec3 GetOctantBits(uint32_t octant) const {
    return glm::ivec3(octant & 0x1, (octant & 0x2) >> 1, (octant & 0x4) >> 2);
  }

  uint32_t OrientationToOctant(const glm::ivec3& orientation) const {
    glm::ivec3 bits = (1 - orientation) >> 1;
    uint32_t octant = bits[0] | (bits[1] << 1) | (bits[2] << 2);
    return octant;
  }

  glm::ivec3 OctantToOrientation(uint32_t octant) const {
    glm::ivec3 bits = glm::ivec3(octant & 0x1, (octant & 0x2) >> 1,
        (octant & 0x4) >> 2);
    glm::ivec3 orientation = 1 - 2 * bits;
    return orientation;
  }
protected:
  typedef typename Octree<SceneObject, SAHOctNode, SAHEncodedNode,
      SAHOctNodeFactory, max_leaf_size, max_depth>::WorkNode WorkNodeType;

  typedef typename Octree<SceneObject, SAHOctNode, SAHEncodedNode,
      SAHOctNodeFactory, max_leaf_size, max_depth>::WorkList WorkListType;

  float GetLeafCost(const ObjectVector& objs, const BoundingBox&) const {
    return K_I * objs.size();
  }

  void InitGrids(SummableGrid<int>* grids, glm::ivec3 size, int k) {
    for (int i = 0; i < k; ++i) {
      grids[i].set_size(size);
      grids[i].Init();
      grids[i].AssignToAll(0);
    }
  }

  void InitGrids(SummableGrid<float>* grids, glm::ivec3 size, int k) {
    for (int i = 0; i < k; ++i) {
      grids[i].set_size(size);
      grids[i].Init();
      grids[i].AssignToAll(0.0f);
    }
  }

  struct CostInfo {
    CostInfo() {
      for (int i = 0; i < 8; ++i) {
        N[i] = 0;
        A[i] = 0.0f;
        bounds[i] = BoundingBox();
      }
    }
    CostInfo(const CostInfo& ci) {
      Copy(ci);
    }
    CostInfo& operator=(const CostInfo& ci) {
      if (this == &ci)
        return *this;
      Copy(ci);
      return *this;
    }
    void Copy(const CostInfo& ci) {
      for (int i = 0; i < 8; ++i) {
        N[i] = ci.N[i];
        A[i] = ci.A[i];
        bounds[i] = ci.bounds[i];
      }
    }
    int N[8];
    float A[8];
    BoundingBox bounds[8];
    float GetCost(float area) const {
      float c = 0.0f;
      for (int i = 0; i < 8; ++i) {
        c += N[i] * A[i];
      }
      c = K_T + K_I * (c / area);
      return c;
    }
  };

  virtual void EvaluateCost2(const ObjectVector& objects,
      const BoundingBox& bounds, float& cost, glm::vec3& split) {
    int num_samples = 4;
    if (objects.size() < (2 << 20) && objects.size() >= (2 << 16))
      num_samples = 4;
    if (objects.size() < (2 << 16) && objects.size() >= (2 << 8))
      num_samples = 4;
    glm::ivec3 size(num_samples, num_samples, num_samples);
    UniformGridSampler sampler(size, bounds);
    SummableGrid<CostInfo> vertex_costs(size);
    glm::ivec3 index = glm::ivec3(0);
    BoundingBox obj_bounds = BoundingBox();

    glm::vec3 point = glm::vec3(0.0f);
    float best_cost = std::numeric_limits<float>::max();
    float current_cost = 0.0f;
    glm::vec3 best_point = glm::vec3(0.0f);
    index = glm::ivec3(0);
    vertex_costs.Init();

    for (int n = 0; n < sampler.num_vertices(); ++n) {
      point = sampler.GetVertexAt(index);
      for (uint32_t octant = 0; octant < 8; ++octant) {
        vertex_costs(index).bounds[octant] = GetOctantBounds(point, bounds,
            octant);
        vertex_costs(index).A[octant] = vertex_costs(index).bounds[octant]
            .GetArea();
      }
      index = GridBase::Step(index, size);
    }
    for (uint32_t i = 0; i < objects.size(); ++i) {
      index = glm::ivec3(0);
      obj_bounds = objects[i]->GetBounds();
      for (int n = 0; n < sampler.num_vertices(); ++n) {
        for (uint32_t octant = 0; octant < 8; ++octant) {
          const BoundingBox& oct_bounds = vertex_costs(index).bounds[octant];
          if (oct_bounds.GetVolume() > 0.0f && obj_bounds.Overlap(oct_bounds))
            ++vertex_costs(index).N[octant];
        }
        index = GridBase::Step(index, size);
      }
    }
    if (test) {
      std::cout << "EvaluateCost2 ---------------------------------"
          << std::endl;
      index = glm::ivec3(0);
      for (int n = 0; n < sampler.num_vertices(); ++n) {
        std::cout << "index = " << index << " P = "
            << sampler.GetVertexAt(index) << " N = ";
        for (uint32_t octant = 0; octant < 8; ++octant)
          std::cout << vertex_costs(index).N[octant] << " ";
        std::cout << " A = ";
        for (uint32_t octant = 0; octant < 8; ++octant)
          std::cout << vertex_costs(index).A[octant] << " ";
        std::cout << " cost = " << vertex_costs(index).GetCost(bounds.GetArea())
            << std::endl;
        index = GridBase::Step(index, size);
      }
    }
    index = glm::ivec3(0);
    float a = bounds.GetArea();
    for (int n = 0; n < sampler.num_vertices(); ++n) {
      current_cost = vertex_costs(index).GetCost(a);
      if (current_cost < best_cost) {
        best_cost = current_cost;
        best_point = sampler.GetVertexAt(index);
      }
      index = GridBase::Step(index, size);
    }
    split = best_point;
    cost = best_cost;
    test = false;
    return;
  }

  virtual void EvaluateCost(const ObjectVector& objects,
      const BoundingBox& bounds, float& cost, glm::vec3& split) {

    if (test2)
      EvaluateCost2(objects, bounds, cost, split);
    int num_samples = 4;
    if (objects.size() < (2 << 20) && objects.size() >= (2 << 16))
      num_samples = 4;
    if (objects.size() < (2 << 16) && objects.size() >= (2 << 8))
      num_samples = 4;
    //if (objects.size() < (2 << 8)) {
    //  split = bounds.GetCenter();
    //  cost = 0.0f;
    //  return;
    //}
    glm::ivec3 size(num_samples, num_samples, num_samples);
    SummableGrid<int> cell_intersections(size - 1);
    UniformGridSampler sampler(size, bounds);
    SummableGrid<float> vertex_costs(size);
    glm::ivec3 index = glm::ivec3(0);
    const SceneObject* obj = NULL;
    BoundingBox obj_bounds = BoundingBox();
    glm::ivec3 bits = glm::ivec3(0);
    glm::vec3 point = glm::vec3(0.0f);
    float best_cost = std::numeric_limits<float>::max();
    float current_cost = 0.0f;
    glm::vec3 best_point = glm::vec3(0.0f);
    index = glm::ivec3(0);

    // populate image integrals
    SummableGrid<int> image_integrals[8];
    InitGrids(&image_integrals[0], size - 1, 8);
    for (uint32_t i = 0; i < objects.size(); ++i) {
      obj = objects[i];
      obj_bounds = obj->GetBounds();
      for (uint32_t octant = 0; octant < 8; ++octant) {
        for (int d = 0; d < 3; ++d)
          point[d] = (
              (octant >> d) & 0x1 ? obj_bounds.max()[d] : obj_bounds.min()[d]);
        if (sampler.PointToCellIndex(point, index))
          ++image_integrals[octant](index);
      }
    }
    if (test2)
      std::cout << "EvaluateCost --------------------------------" << std::endl;
    //  sample each counting function using image integral
    for (uint32_t octant = 0; octant < 8; ++octant)
      image_integrals[octant].OrientedImageIntegral(
          OctantToOrientation(octant));

    // find lowest cost vertex
    float area = 0.0f;
    int count = 0;
    BoundingBox octant_bounds = BoundingBox();
    index = glm::ivec3(0);
    for (int n = 0; n < sampler.num_vertices(); ++n) {
      point = sampler.GetVertexAt(index);
      current_cost = 0.0f;
      if (test2) {
        std::cout << "index = " << index << " P = " << point << " N = ";
        for (uint32_t octant = 0; octant < 8; ++octant) {
          std::cout
              << image_integrals[octant].GetSafe(
                  index + GetOctantBits(octant) - 1, 0) << " ";
        }
        std::cout << " A = ";
        for (uint32_t octant = 0; octant < 8; ++octant) {
          std::cout << GetOctantBounds(point, bounds, octant).GetArea() << " ";
        }
      }
      for (uint32_t octant = 0; octant < 8; ++octant) {
        bits = GetOctantBits(octant);
        octant_bounds = GetOctantBounds(point, bounds, octant);
        area = octant_bounds.GetArea();
        count = image_integrals[octant].GetSafe(index + bits - 1, 0);
        current_cost += area * count;
      }
      current_cost = K_T + K_I * (current_cost / bounds.GetArea());
      if (test2) {
        std::cout << " cost = " << current_cost << std::endl;
        /**for (uint32_t octant = 0; octant < 8; ++octant) {
         std::cout << " bounds = " << GetOctantBounds(point, bounds, octant)
         << " A = " << GetOctantBounds(point, bounds, octant).GetArea()
         << std::endl;
         }
         for (uint32_t octant = 0; octant < 8; ++octant) {
         std::cout << "cell_index = " << index - GetOctantBits(octant)
         << " bits = " << GetOctantBits(octant) << std::endl;
         }**/
      }
      if (current_cost < best_cost) {
        best_point = point;
        best_cost = current_cost;
      }
      index = GridBase::Step(index, size);
    }
    test2 = false;
    split = best_point;
    cost = best_cost;
  }

  virtual void BuildRoot(SAHOctNode& root, WorkNodeType& work_root) {
    glm::vec3 split = glm::vec3(0.0f);
    float cost = 0.0f;
    EvaluateCost(work_root.objects, work_root.bounds, cost, split);
    if (cost > this->GetLeafCost(work_root.objects, work_root.bounds)
        || (0 == max_depth) || (work_root.objects.size() <= max_leaf_size))
      root = this->GetNodeFactory().CreateLeaf(0);
    else {
      root = this->GetNodeFactory().CreateInternal(0);
      root.set_point(split);
    }
  }

  virtual void BuildInternal(SAHOctNode& node, WorkNodeType& work_node,
      WorkListType& next_list, uint32_t depth) {
    ++this->num_internal_nodes_;
    WorkNodeType child_work_nodes[8]; // process children tentatively
    node.set_offset(this->nodes_.size()); // children will have nodes pushed
    for (uint32_t j = 0; j < 8; ++j)
      child_work_nodes[j] = WorkNodeType( // initialize child lists
          this->GetChildBounds(node, work_node.bounds, j));
    while (!work_node.objects.empty()) {
      const SceneObject* obj = work_node.objects.back();
      work_node.objects.pop_back();
      for (uint32_t j = 0; j < 8; ++j)  // distribute to children
        if (obj->GetBounds().Overlap(child_work_nodes[j].bounds))
          child_work_nodes[j].objects.push_back(obj);
    }
    for (uint32_t j = 0; j < 8; ++j) {
      // If a child has a non-empty object list, process it.
      if (child_work_nodes[j].objects.size() > 0) {
        node.set_size(node.size() + 1); // update parent size
        uint32_t count = child_work_nodes[j].objects.size();
        glm::vec3 split = glm::vec3(0.0f);
        float cost = 0.0f;
        float leaf_cost = this->GetLeafCost(child_work_nodes[j].objects,
            child_work_nodes[j].bounds);
        EvaluateCost(child_work_nodes[j].objects, child_work_nodes[j].bounds,
            cost, split);
        SAHOctNode child;
        if (cost > leaf_cost || depth + 1 >= this->GetMaxDepth()
            || count <= this->GetMaxLeafSize())
          child = this->GetNodeFactory().CreateLeaf(j);
        else {
          child = this->GetNodeFactory().CreateInternal(j);
          child.set_point(split);
        }
        child_work_nodes[j].node_index = this->nodes_.size();
        next_list.push_back(child_work_nodes[j]);
        this->nodes_.push_back(this->EncodeNode(child));
      }
    }
  }
};
}
#endif /* SAH_OCTREE_HPP_ */
