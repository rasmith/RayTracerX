/*
 * ray.hpp
 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */

#ifndef RAY_HPP_
#define RAY_HPP_
namespace ray {
class Ray {
public:
    Ray();
    Ray(const glm::vec3 &origin, const glm::vec3 &direction);
    Ray(const Ray& ray);
    glm::vec3 operator()(float t) const;
    const glm::vec3& direction() const;
    const glm::vec3& origin() const;
private:
    glm::vec3 origin_;
    glm::vec3 direction_;
};
} // namespace ray
#endif /* RAY_HPP_ */
