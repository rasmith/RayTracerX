/*
 * accelerator.hpp
 *
 *  Created on: Apr 24, 2014
 *      Author: randallsmith
 */
#ifndef ACCELERATOR_H_
#define ACCELERATOR_H_
#include "scene.hpp"
namespace ray {
class Accelerator: public SceneShape {
public:
  virtual ~Accelerator();
protected:
  Accelerator();
};
} // namespace ray
#endif
