/*
 * raytracer.cpp
 *
 *  Created on: Nov 14, 2013
 *      Author: agrippa
 */
#include <string>

#include "camera.hpp"
#include "io_utils.hpp"
#include "raytracer.hpp"
#include "scene.hpp"
namespace ray {
static int hit_count = 0;
static int miss_count = 0;

RayTracer::RayTracer() :
    scene_(NULL), camera_(NULL), background_color_(glm::vec3(0.0f)),
        display_progress_(true), display_stats_(true) {
}

RayTracer::RayTracer(Scene* scene, Camera* camera) :
    scene_(scene), camera_(camera), background_color_(glm::vec3(0.0f)),
        display_progress_(true), display_stats_(true) {
}

const glm::vec3& RayTracer::background_color() const {
  return background_color_;
}

void RayTracer::set_background_color(const glm::vec3& background_color) {
  background_color_ = background_color;
}

void RayTracer::Render(Image& image) {
  hit_count = 0;
  miss_count = 0;
  int width = camera_->screen_width();
  int height = camera_->screen_height();
  int img_size = width * height;
  int count = 0;
  int progress_increment = 1;
  int current_progress = 0;
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      image(i, j) = TraceRay(j, i);
      ++count;
      int progress = round(
          static_cast<float>(count) / static_cast<float>(img_size) * 100.0f);
      if (display_progress_
          && progress >= current_progress + progress_increment) {
        std::cout << " " << progress << std::flush;
        current_progress = progress;
      }
    }
  }
  if (display_stats_) {
    std::cout << "\n";
    std::cout << "hits = " << hit_count << " misses = " << miss_count
        << std::endl;
  }
}

float RayTracer::Diffuse(const Isect& isect, const Light& light) const {
  glm::vec3 P = isect.ray(isect.t_hit);
  glm::vec3 L = Light::Direction(light, P); // direction to light source
  float dotNL = std::max(0.0f, glm::dot(isect.normal, L));
  return dotNL;
}

float RayTracer::Specular(const Isect& isect, const Light& light) const {
  glm::vec3 P = isect.ray(isect.t_hit);
  glm::vec3 L = Light::Direction(light, P);
  glm::vec3 R = glm::normalize(glm::reflect(-L, isect.normal));
  glm::vec3 V = glm::normalize(isect.ray.origin() - P); // direction to viewer
  float dotRV = std::max(0.0f, glm::dot(R, V));
  float spec = static_cast<float>(pow(dotRV, isect.mat->ns));
  return spec;
}

float RayTracer::Attenuate(const Isect& isect, const Light& light) const {
  float a = light.attenuation_coefficients[0];
  float b = light.attenuation_coefficients[1];
  float c = light.attenuation_coefficients[2];
  float d = glm::distance(isect.ray(isect.t_hit), light.ray.origin());
  float f_atten = 1.0f / (a + b * d + c * d * d);
  return std::min(1.0f, f_atten);
}

glm::vec3 RayTracer::Shade(const Isect& isect) const {
  glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
  for (uint32_t i = 0; i < scene_->lights().size(); ++i) {
    const Light& light = scene_->lights()[i];
    color += isect.mat->kd * light.kd * Diffuse(isect, light);
    color += isect.mat->ks * light.ks * Specular(isect, light);
    color = color * Attenuate(isect, light);
    color = glm::clamp(color, 0.0f, 1.0f);
  }
  return color;
}

glm::vec3 RayTracer::TraceRay(int pixel_x, int pixel_y) const {
  float x = pixel_x;
  float y = pixel_y;
  Ray ray = camera_->GenerateRay(x, y);
  //if(x == 535 && y == 512)  std::cout << "ray = " << ray <<std::endl;
  return TraceRay(ray);
}

glm::vec3 RayTracer::TraceRay(const Ray& ray) const {
  glm::vec3 color = background_color_;
  Isect isect;
  bool hit = scene_->Intersect(ray, isect);
  if (hit) {
    color = Shade(isect);
    //color = 0.5f * (isect.normal + 1.0f);
    //std::cout << "normal = " << isect.normal << std::endl;
    ++hit_count;
  } else
    ++miss_count;
  return 255.0f * color;
}

bool RayTracer::display_progress() const {
  return display_progress_;
}

void RayTracer::set_display_progress(bool display_progress) {
  display_progress_ = display_progress;
}

bool RayTracer::display_stats() const {
  return display_stats_;
}

void RayTracer::set_display_stats(bool display_stats) {
  display_stats_ = display_stats;
}

void RayTracer::set_scene(Scene* scene) {
  scene_ = scene;
}

void RayTracer::set_camera(Camera* camera) {
  camera_ = camera;
}
} // namespace ray
