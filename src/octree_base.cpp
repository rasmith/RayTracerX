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
const uint32_t OctreeBase::kMaxDepth = 8;
const uint32_t OctreeBase::kMaxLeafSize = 32;
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
