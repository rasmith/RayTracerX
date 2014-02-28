/*
 * types.hpp
 *
 *  Created on: Sep 30, 2013
 *      Author: agrippa
 */
#ifndef TYPES_HPP_
#define TYPES_HPP_
#define GLM_FORCE_PURE
#define GLM_FORCE_CXX98
#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/detail/precision.hpp>
namespace ray {
typedef glm::detail::tvec3<unsigned char, glm::highp> ucvec3;
} // namespace ray
#endif /* TYPES_HPP_ */
