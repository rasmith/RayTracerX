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
  const glm::vec3& background_color() const;
  void set_background_color(const glm::vec3& background_color);
  bool display_progress() const;
  void set_display_progress(bool display_progress);
  bool display_stats() const;
  void set_display_stats(bool display_stats);
  void set_scene(Scene* scene);
  void set_camera(Camera* camera);
private:
  float Diffuse(const Isect& isect, const Light& light) const;
  float Specular(const Isect& isect, const Light& light) const;
  float Attenuate(const Isect& isect, const Light& light) const;
  glm::vec3 Shade(const Isect& isect) const;
  glm::vec3 TraceRay(int pixel_x, int pixel_y) const;
  glm::vec3 TraceRay(const Ray& ray) const;
  Scene* scene_;
  Camera* camera_;
  glm::vec3 background_color_;
  bool display_progress_;
  bool display_stats_;
};
} // namespace ray
#endif /* RAYTRACER_HPP_ */
