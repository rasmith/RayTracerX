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
class OctTree : public Shape {
public:
    void Build(const std::vector<Shape*>& objects);
    void Build(const std::list<Shape*>& objects);
private:
    void BuildRecursive(std::list<Shape*>& objects);
};
}

#endif /* OCTREE_HPP_ */
