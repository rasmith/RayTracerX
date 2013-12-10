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
RayTracer::RayTracer(Scene* scene, Camera* camera) :
        scene_(scene), camera_(camera) {
}

void RayTracer::Render(Image& image) {
    int width = camera_->screen_width();
    int height = camera_->screen_height();
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            image(i, j) = TraceRay(j, i);
        }
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
    return TraceRay(ray);
}

glm::vec3 RayTracer::TraceRay(const Ray& ray) const {
    glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
    Isect isect;
    bool hit = scene_->Intersect(ray, isect);
    if (hit) {
        color = Shade(isect);
       // std::cout << " N = " << isect.normal << " color = " << color
       //         << std::endl;
    }
    return 255.0f * color;
}

} // namespace ray

