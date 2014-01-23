/*
 * octree.hpp
 *
 *  Created on: Dec 19, 2013
 *      Author: agrippa
 */

#ifndef OCTREE_HPP_
#define OCTREE_HPP_
#include <algorithm>
#include <cstring>
#include <list>
#include <vector>
#include "shape.hpp"
namespace ray {
class OctTreeBase : public Shape {
public:
	static const int kMaxDepth;
	static const int kNodeSizeBytes;

	enum NodeType {kInternal = 0, kLeaf = 1, kNumNodeTypes};
private:
	OctTreeBase();
};
template<class ChildType>
struct OctNode {
	OctTreeBase::NodeType type;
	ChildType* children;
	int size;
	short octant;
};
template<class SceneObject>
class OctTree: public OctTreeBase {
private:


	struct OctNode {
		unsigned char data[kNodeSizeBytes];
		OctNode() : index(0) {
		}
		bool Intersect(const Ray& ray, Isect& isect) const {
			float t_near = -std::numeric_limits<float>::max();
			float t_far = std::numeric_limits<float>::max();
			bool hit = bounds.Intersect(ray, t_near, t_far);
			isect.t_hit = t_near;
			return hit;
		}
	};
	struct OctLeaf {
		ObjectIndex index;
		bool Intersect(const ObjectList* objs, const Ray& ray, Isect& isect) const {
			bool hit = false;
			Isect current, best;
			best.t_hit = std::numeric_limits<float>::max();
			for (uint32_t i = 0; i < objs.size(); ++i) {
				if (objs[i]->Intersect(ray, current)
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
	};
public:
	virtual ~OctTree();
	virtual bool Intersect(const Ray& ray, Isect& isect) const {
		return Traverse(root_index_, ray, isect, 0);
	}
	virtual BoundingBox GetBounds() = 0;
	virtual void Print(std::ostream& out) const;
	void Build(const ObjectVector& objects) {

	}
private:
	std::vector<OctNode> nodes_;
	std::vector<OctIndex> indices_;
	std::vector<OctLeaf> leaves_;
	ObjectVector scene_objects_;
	const OctNode& GetNodeConstRef(const OctIndex& index) const {
		return root_index_;
	}
	OctIndex root_index_;
	BoundingBox root_bounds_;
	bool Traverse(const OctIndex& node_index, const Ray& ray, Isect& isect,
			int depth) const {
		const OctNode& node = GetNodeConstRef(node_index);
		if (!node.Intersect(ray, isect))
			return false;
		return true;
	}
	void BuildRecursive(ObjectVector& objects);
};

}

#endif /* OCTREE_HPP_ */
