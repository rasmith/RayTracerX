/*
 * raytracer.cpp
 *
 *  Created on: Nov 14, 2013
 *      Author: agrippa
 */
#include "camera.hpp"
#include "raytracer.hpp"
#include "scene.hpp"
namespace ray {
RayTracer::RayTracer(const Scene* scene, const Camera* camera) :
        scene_(scene), camera_(camera) {
}

void RayTracer::Render(Image& image) {
    int width = camera_->screen_width();
    int height = camera_->screen_height();
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            image(i, j) = TraceRay(i, j);
        }
    }
}

glm::vec3 RayTracer::Diffuse(const Isect& isect, const Light& light) const {
    glm::vec3 color = light.kd * isect.mat->kd;
    float dotNL = glm::dot(glm::normalize(isect.normal),
            glm::normalize(light.ray.direction()));
    color = dotNL * color;
    return color;
}

glm::vec3 RayTracer::Specular(const Isect& isect, const Light& light) const {
    glm::vec3 color = light.ks * isect.mat->ks;
    glm::vec3 R = glm::reflect(light.ray.direction(), isect.normal);
    glm::vec4 D = camera_->direction();
    glm::vec3 V = glm::normalize(glm::vec3(D[0], D[1], D[2]));
    float dotRV = glm::dot(glm::normalize(R), V);
    color = pow(dotRV, isect.mat->ns)  * color;
    return color;
}

glm::vec3 RayTracer::Shade(const Isect& isect) const {
    glm::vec3 color = glm::vec3(0.0f);
    for (int i = 0; i < scene_->lights().size(); ++i) {
        const Light& light = scene_->lights()[i];
        color += Diffuse(isect, light);
        color += Specular(isect, light);
    }
    return color;
}

glm::vec3 RayTracer::TraceRay(int pixel_x, int pixel_y) const {
    float x = pixel_x;
    float y = pixel_y;
    Ray ray = camera_->GenerateRay(x, y);
    return TraceRay(ray, 0);
}

glm::vec3 RayTracer::TraceRay(const Ray& ray, int depth) const {
    glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
    Isect isect;
    bool hit = scene_->Intersect(ray, isect);
    if (hit) {
        color = Shade(isect);
    }
    return color;
}

} // namespace ray

