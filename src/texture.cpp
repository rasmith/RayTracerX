/*
 * texture.cpp
 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */
#include "texture.hpp"
#include "image.hpp"
namespace ray {
TexCoord::TexCoord() {
}

TexCoord::TexCoord(const TexCoord& texcoord) :
        coords(texcoord.coords) {
}

TexCoord::TexCoord(const glm::vec3& values) :
        coords(values) {
}

TexInfo::TexInfo() {
}
} // namespace ray

