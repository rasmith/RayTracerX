/*
 * texture.hpp
 *
 *  Created on: Oct 3, 2013
 *      Author: agrippa
 */

#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_
#include "image.hpp"
namespace ray {
struct TexCoord {
    TexCoord();
    TexCoord(const TexCoord& texcoord);
    TexCoord(const glm::vec3& values);
    glm::vec3 coords;
};
struct TexInfo {
    TexInfo();
    std::string filepath;
    Image image;
};
} // namespace ray
#endif /* TEXTURE_HPP_ */
