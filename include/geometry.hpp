/*
 * geom.hpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */

#ifndef GEOM_HPP_
#define GEOM_HPP_

#include "types.hpp"

namespace ray {
namespace geom {

struct Triangle {
    Triangle();
    Triangle(const Triangle& triangle);
    Triangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
    glm::vec3 vertices[3];
};

}
}

#endif /* GEOM_HPP_ */
