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
template<class SceneObject, KdNode, KdEncodedNode, KdNodeFactory>
class Kdtree: public TreeBase<SceneObject, KdNode, KdEncodedNode, KdNodeFactory> {
public:
  typedef std::vector<const SceneObject*> ObjectVector;

  Kdtree() :
      TreeBase<SceneObject, KdNode, KdEncodedNode, KdNodeFactory>::TreeBase() {
  }
protected:
  typedef typename TreeBase<SceneObject, KdNode, KdEncodedNode,
      KdNodeFactory>::WorkNode WorkNodeType;
  typedef typename TreeBase<SceneObject, KdNode, KdEncodedNode,
        KdNodeFactory>::WorkList WorkListType;

  virtual BoundingBox GetChildBounds(const KdNode& node,
      const BoundingBox& bounds, uint32_t index) const {
    BoundingBox child_bounds;
    return child_bounds;
  }

  virtual KdNode GetIthChildOf(const KdNode& node, uint32_t index) const {
    KdNode child;
    return child;
  }

  virtual void BuildRoot(KdNode& root, WorkNodeType& work_root) {
  }

  virtual void BuildLeaf(KdNode& node, WorkNodeType& work_node) {
  }

  virtual void BuildInternal(KdNode& node, KdNode& work_node,
      WorkList& next_list, uint32_t depth) {
  }

  virtual void BuildInternal(KdNode& node, WorkNodeType& work_node,
      WorkList& next_list, uint32_t depth) {
  }
};

#endif /* KDTREE_HPP_ */
