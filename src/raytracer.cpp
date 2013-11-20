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

glm::vec3 RayTracer::Shade(const Isect& isect) const {
    return glm::vec3(0.0f);
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
    	bool reflective = false;
    	bool refractive = false;
    	if (reflective) {
    		color += isect.mat.kr * TraceRay(Reflect(isect), depth + 1);
    	}
    	if (refractive)  {
    		color += isect.mat.kt * TraceRay(Refract(isect), depth + 1);
    	}
    }
    return color;
}

} // namespace ray

