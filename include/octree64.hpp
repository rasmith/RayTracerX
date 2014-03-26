/*
 * octree64.hpp
 *
 *  Created on: Mar 21, 2014
 *      Author: randallsmith
 */

#ifndef OCTREE64_HPP_
#define OCTREE64_HPP_
#include "octree.hpp"
#include "octree_base.hpp"
#include "octnode64.hpp"
namespace ray {
template<class SceneObject, int max_leaf_size, int max_depth>
class Octree64: public Octree<SceneObject, OctNode64, EncodedNode64,
    OctNodeFactory64, max_leaf_size, max_depth> {
public:
  Octree64() :
          Octree<SceneObject, OctNode64, EncodedNode64, OctNodeFactory64,
              max_leaf_size, max_depth>::Octree() {
  }
  virtual ~Octree64() {
  }
};
}
#endif /* OCTREE64_HPP_ */
