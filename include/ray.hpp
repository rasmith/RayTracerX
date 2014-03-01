/*
 * ray.hpp
 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */

#ifndef RAY_HPP_
#define RAY_HPP_
#include <ostream>
#include <glm/glm.hpp>
#include "types.hpp"
namespace ray {
class Ray {
public:
  Ray();
  Ray(const glm::vec3 &origin, const glm::vec3 &direction);
  Ray(const Ray& ray);
  Ray& operator=(const Ray& ray);
  glm::vec3 operator()(float t) const;
  const glm::vec3& direction() const;
  const glm::vec3& origin() const;
  Ray operator-() const;
  bool operator==(const Ray& r) const;
private:
  glm::vec3 origin_;
  glm::vec3 direction_;
};
std::ostream& operator<<(std::ostream& out, const Ray& r);
} // namespace ray
#endif /* RAY_HPP_ */
