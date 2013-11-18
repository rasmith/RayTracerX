/*
 * raytracer.hpp
 *
 *  Created on: Nov 12, 2013
 *      Author: agrippa
 */

#ifndef RAYTRACER_HPP_
#define RAYTRACER_HPP_
#include "types.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "transform.hpp"
#include "image.hpp"
namespace ray {
class RayTracer {
public:
    RayTracer();
    RayTracer(const Scene* scene, const Camera* camera);
    void Render(Image& image);
private:
    glm::vec3 TraceRay(int pixel_x, int pixel_y);
    Scene* scene_;
    Camera* camera_;
};
} // namespace ray
#endif /* RAYTRACER_HPP_ */
