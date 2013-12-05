/*
 * raytracer.hpp
 *
 *  Created on: Nov 12, 2013
 *      Author: agrippa
 */

#ifndef RAYTRACER_HPP_
#define RAYTRACER_HPP_
#include "scene.hpp"
#include "camera.hpp"
#include "image.hpp"
#include "transform.hpp"
#include "types.hpp"
namespace ray {
class RayTracer {
public:
    RayTracer();
    RayTracer(Scene* scene, Camera* camera);
    void Render(Image& image);
private:
    float Diffuse(const Isect& isect, const Light& light) const;
    float Specular(const Isect& isect, const Light& light) const;
    float Attenuate(const Isect& isect, const Light& light) const;
    glm::vec3 Shade(const Isect& isect) const;
    glm::vec3 TraceRay(int pixel_x, int pixel_y) const;
    glm::vec3 TraceRay(const Ray& ray) const;
    Scene* scene_;
    Camera* camera_;
};
} // namespace ray
#endif /* RAYTRACER_HPP_ */
