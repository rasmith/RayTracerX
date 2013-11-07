/*
 * io_utils.hpp
 *
 *  Created on: Nov 6, 2013
 *      Author: agrippa
 */

#ifndef IO_UTILS_HPP_
#define IO_UTILS_HPP_
#include <iostream>
#include <glm/glm.hpp>
namespace ray {
std::ostream& operator<<(std::ostream& out, glm::vec3 v);
std::ostream& operator<<(std::ostream& out, glm::vec4 v);
std::ostream& operator<<(std::ostream& out, glm::mat4x4 m);
} // namespace ray
#endif /* IO_UTILS_HPP_ */
