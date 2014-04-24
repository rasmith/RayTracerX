/*
 * kdtree64.hpp
 *
 *  Created on: Apr 23, 2014
 *      Author: randallsmith
 */
#ifndef KDREE64_HPP_
#define KDREE64_HPP_
#include "kdtree.hpp"
#include "kdnode64.hpp"
namespace ray {
template<class SceneObject>
class Kdtree64: public Kdtree<SceneObject, KdNode64, EncodedKdNode64,
    KdNodeFactory64> {
public:
  Kdtree64() :
      Kdtree<SceneObject, KdNode64, EncodedKdNode64, KdNodeFactory64>::Kdtree() {
  }
  virtual ~Kdtree64() {
  }
};
}
#endif /* KDTREE64_HPP_ */

