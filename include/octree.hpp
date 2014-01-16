/*
 * octree.hpp
 *
 *  Created on: Dec 19, 2013
 *      Author: agrippa
 */

#ifndef OCTREE_HPP_
#define OCTREE_HPP_
#include <algorithm>
#include <list>
#include <vector>
#include "shape.hpp"
namespace ray {
template<class SceneObject>
class OctTree: public Shape {
private:
	typedef std::vector<SceneObject*> ObjectVector;
	typedef std::list<SceneObject*> ObjectList;
	struct OctNode {
		BoundingBox bounds;
		OctNode children[8];
		bool Intersect(const Ray& ray, Isect& isect) const {
			float t_near = -std::numeric_limits<float>::max();
			float t_far = std::numeric_limits<float>::max();
			bool hit = bounds.Intersect(ray, t_near, t_far);
			isect.t_hit = t_near;
			return hit;
		}
	};
	struct OctLeaf {
		ObjectList* objects;
		bool Intersect(const Ray& ray, Isect& isect) const {
			const ObjectList& objs = *objects;
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
	virtual bool Intersect(const Ray& ray, Isect& isect) const = 0;
	virtual BoundingBox GetBounds() = 0;
	virtual void Print(std::ostream& out) const;
	void Build(const ObjectVector& objects) {

	}
private:
	void BuildRecursive(ObjectVector& objects);
};
}

#endif /* OCTREE_HPP_ */
