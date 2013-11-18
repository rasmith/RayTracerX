/*
 * raytracer.cpp
 *
 *  Created on: Nov 14, 2013
 *      Author: agrippa
 */
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

glm::vec3 RayTracer::TraceRay(int pixel_x, int pixel_y) {
    return glm::vec3(0.0f, 0.0f, 0.0f);
}

} // namespace ray

