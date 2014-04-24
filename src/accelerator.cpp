/*
 * accelerator.cpp
 *
 *  Created on: Apr 24, 2014
 *      Author: randallsmith
 */
#include "accelerator.hpp"
#include "scene.hpp"
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
}

