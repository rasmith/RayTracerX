/*
 * octree.hpp
 *
 *  Created on: Dec 19, 2013
 *      Author: agrippa
 */

#ifndef OCTREE_HPP_
#define OCTREE_HPP_
#include <stdint.h>
#include <algorithm>
#include <cstring>
#include <list>
#include <vector>
#include "shape.hpp"
namespace ray {
class OctTreeBase: public Shape {
public:
	static const int kMaxDepth;
	static const int kMaxLeafSize;
	enum NodeType {
		kInternal = 0, kLeaf = 1
	};
private:
	OctTreeBase();
};

template<class SceneObject>
class OctTree: public OctTreeBase {
private:
	typedef std::vector<SceneObject*> ObjectVector;
	// data[0]: top bit is type
	//		  : next three bits indicate octant
	//		  : bottom four bits unused
	// data[1]: size of node, 8 bits, max size of 256, empty nodes not stored
	// data[2..5]: offset into corresponding array, 32 bits
	struct EncodedNode {
		u_char data[8];
		NodeType GetType() const {
			return static_cast<NodeType>((data[0] & 0x80u) >> 7);
		}
		uint32_t GetOctant() const {
			return static_cast<uint32_t>((data[0] & 0x30u) >> 4);
		}
		uint32_t GetSize() const {
			return static_cast<uint32_t>(data[1]);
		}
		uint32_t GetOffset() const {
			uint32_t offset = 0x0u;
			uint32_t byte = 0x0u;
			for (uint32_t i = 2; i < 6; ++i) {
				byte = static_cast<uint32_t>(data[i]);
				offset = offset | (byte << ((i - 2) * 8));
			}
			return offset;
		}
		void SetType(NodeType type) {
			u_char mask = static_cast<u_char>(type);
			mask = (mask << 7) & 0x80u;
			data[0] = data[0] | mask;
		}
		void SetOctant(uint32_t octant) {
			u_char mask = static_cast<u_char>(octant);
			mask = (mask << 4) & 0x30u;
			data[0] = data[0] | mask;
		}
		void SetSize(uint32_t size) {
			u_char mask = static_cast<u_char>(size);
			data[1] = mask;
		}
		void SetOffset(uint32_t offset) {
			uint32_t byte = 0x0u;
			uint32_t shift = 0;
			for (uint32_t i = 2; i < 6; ++i) {
				shift = (i - 2) * 8;
				byte = (offset >> shift) & 0x0000000Fu;
				data[i] = static_cast<u_char>(byte);
			}
		}
	};
	struct OctNode {
		OctNode(const NodeType& node_type, uint32_t node_octant,
				uint32_t node_size, uint32_t node_offset) :
				type(node_type), octant(node_octant), size(node_size), offset(
						node_offset) {
		}
		NodeType type;
		uint32_t octant;
		uint32_t size;
		uint32_t offset;
	};
	OctNode DecodeNode(const EncodedNode& encoded) const {
		OctNode node;
		uint32_t offset = encoded.GetOffset();
		node.size = encoded.GetSize();
		node.octant = encoded.GetOctant();
		node.type = encoded.GetType();
		node.offset = encoded.GetOffset();
		return node;
	}
	EncodedNode EncodeNode(const OctNode& node) const {
		EncodedNode encoded;
		encoded.SetType(node.type);
		encoded.SetOctant(node.octant);
		encoded.SetOffset(node.offset);
		encoded.SetSize(node.size);
		return encoded;
	}
	uint32_t PointToOctant(const BoundingBox& bounds, const glm::vec3& point) {
		glm::vec3 center = bounds.GetCenter();
		uint32_t x_bit = (point[0] > center[0]);
		uint32_t y_bit = (point[1] > center[1]);
		uint32_t z_bit = (point[2] > center[2]);
		uint32_t octant = x_bit | (y_bit << 1) | (z_bit << 2);
		return octant;
	}
	BoundingBox GetChildBounds(const BoundingBox& bounds,
			uint32_t octant) const {
		BoundingBox child_bounds;
		return child_bounds;
	}
public:
	virtual ~OctTree();
	virtual bool Intersect(const Ray& ray, Isect& isect) const {
		return Traverse(nodes_[0], bounds_, ray, isect, 0);
	}
	virtual BoundingBox GetBounds() = 0;
	virtual void Print(std::ostream& out) const;
	void Build(const ObjectVector& objects) {
		BuildTree(objects);
	}
private:
	std::vector<EncodedNode> nodes_;
	ObjectVector scene_objects_;
	BoundingBox bounds_;
	bool IntersectLeaf(const OctNode& leaf, const BoundingBox& bounds,
			const Ray& ray, Isect& isect) {
		bool hit = false;
		Isect current;
		Isect best;
		best.t_hit = std::numeric_limits<float>::max();
		SceneObject* const objects = scene_objects_[leaf.size];
		for (uint32_t i = 0; i < leaf.size(); ++i) {
			if (objects[i]->Intersect(ray, current)
					&& current.t_hit < best.t_hit) {
				best = current;
				hit = true;
			}
		}
		if (hit) {
			isect = best;
		}
		return hit;
	}
	bool Traverse(const OctNode& node, const BoundingBox& bounds,
			const Ray& ray, Isect& isect, int depth) const {
		if (depth > kMaxDepth) // check depth first
			return false;
		float t_near, t_far;
		if (!bounds.Intersect(ray, t_near, t_far)) // check bounds next
			return false;
		if (kLeaf == node.type) // is this a leaf?
			return IntersectLeaf(node, bounds, ray, isect);
		// This is an internal node:  Decode each child and
		// do AABB intersection and sort the hit children by
		// t_near times.  Return first child that records an
		// object intersection.
		OctNode candidates[4]; // can hit at most four children
		BoundingBox child_bounds[4];
		float t_near_vals[4];
		float t_far_vals[4];
		int count = 0;
		EncodedNode* const children = &nodes_[node.offset];
		for (int i = 0; count < 4 && i < node.size; ++i) {
			candidates[count] = DecodeNode(children[i]);
			child_bounds[count] = GetChildBounds(bounds, candidates[count]);
			if (child_bounds[count].Intersect(ray, t_near_vals[count],
					t_far_vals[count]))
				++count;
		}
		// sort by t_near - selection sort
		for (int i = 0; i < count; ++i) {
			int *pos = std::min_element(t_near_vals + i, t_near_vals + count);
			int k = pos - &count[0];
			std::swap(candidates[i], candidates[k]);
			std::swap(child_bounds[i], child_bounds[k]);
			std::swap(t_near_vals[i], t_far_vals[k]);
			std::swap(t_far_vals[i], t_far_vals[k]);
		}
		bool hit = false;
		for (int i = 0; i < count && !hit; ++i) {
			hit = Traverse(candidates[i], child_bounds[i], ray, isect,
					depth + 1);
		}
		return hit;
	}
	struct WorkNode {
		WorkNode(const BoundingBox& bbox) :
				node_index(0), bounds(bbox), objects() {
			objects.clear();
		}
		uint32_t node_index;
		BoundingBox bounds;
		ObjectVector objects;
	};
	typedef std::vector<WorkNode> WorkList;
	void BuildLeaf(OctNode& node, WorkNode& work_node, WorkList& next_list,
			int depth) {
		node.offset = scene_objects_.size();
		node.size = work_node.objects.size();
		while (!work_node.objects.empty()) {
			scene_objects_.push_back(work_node.objects.back());
			work_node.objects.pop_back();
		}
	}
	void BuildInternal(OctNode& node, WorkNode& work_node, WorkList& next_list,
			int depth) {
		WorkNode child_work_nodes[8]; // process children tentatively
		node.offset = nodes_.size(); // children will have nodes pushed
		for (uint32_t j = 0; j < 8; ++j) {
			child_work_nodes[j] = WorkNode( // initialize child lists
					GetChildBounds(work_node.bounds, j));
		}
		while (!work_node.objects.empty()) {
			SceneObject* obj = work_node.objects.back();
			work_node.objects.pop_back();
			for (uint32_t j = 0; j < 8; ++j) { // distribute to children
				if (obj->GetBounds().Overlap(child_work_nodes[j].bounds))
					child_work_nodes[j].objects.push_back(obj);
			}
		}
		for (uint32_t j = 0; j < 8; ++j) {
			// If a child has a non-empty object list, process it.
			if (child_work_nodes[j].object_list.size() > 0) {
				++node.size; // update parent size
				uint32_t count = child_work_nodes[j].objects.size();
				OctNode child;
				if (depth > kMaxDepth || count <= kMaxLeafSize)
					child = OctNode(kLeaf, j, 0, 0);
				else
					child = OctNode(kInternal, j, 0, 0);
				child_work_nodes[j].node_index = nodes_.size();
				next_list.push_back(child_work_nodes[j]);
				nodes_.push_back(EncodeNode(child));
			}
		}
	}
	void BuildLevel(WorkList& work_list, WorkList& next_list, int depth) {
		while (!work_list.empty()) {
			WorkNode work_node = work_list.back();
			work_list.pop_back();
			OctNode node = DecodeNode(nodes_[work_node.node_index]);
			if (kLeaf == node.type) {
				BuildLeaf(node, work_node, next_list, depth);
			} else {
				BuildInternal(node, work_node, next_list, depth);
			}
			nodes_[work_node.node_index] = EncodeNode(node);
		}
	}
	void BuildTree(const ObjectVector& objects) {
		std::vector<WorkNode> work_list;
		std::vector<WorkNode> next_list;
		int depth = 0;
		// Create and insert root
		OctNode root = OctNode(kInternal, 0, 0, 0);
		nodes_.push_back(EncodeNode(root));
		WorkNode work_root = WorkNode(bounds_);
		for (int i = 0; i < objects.size(); ++i)
			work_root.objects.push_back(objects[i]);
		work_root.node_index = 0;
		work_list.push_back(work_root);
		next_list.clear();
		// This is a breadth first build. There are two work lists: work_list
		// and next_list.  The work_list gets swapped when empty whilecnext_list
		// fills up. Each time this happens, one level has been completed.
		while (!work_list.empty()) {
			BuildLevel(work_list, next_list, depth);
			work_list.swap(next_list);
			++depth;
		}
	}
}
;

}

#endif /* OCTREE_HPP_ */
