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
  virtual bool trace() const;
  virtual void set_trace(bool trace);
protected:
  Accelerator();
  bool trace_;
};
} // namespace ray
#endif
