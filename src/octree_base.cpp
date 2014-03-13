/*
 * octree_base.cpp
 *
 *  Created on: Feb 5, 2014
 *      Author: agrippa
 */
#include <sys/types.h>
#include "io_utils.hpp"
#include "octree_base.hpp"
#include "scene.hpp"
#include "shape.hpp"
namespace ray {
Accelerator::~Accelerator() {
}

Accelerator::Accelerator() :
    SceneShape(), trace_(false) {
}

bool Accelerator::trace() const {
  return trace_;
}

void Accelerator::set_trace(bool trace) {
  trace_ = trace;
}
}  // namespace ray
