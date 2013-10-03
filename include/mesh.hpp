/*
 * mesh.hpp
 *
 *  Created on: Oct 1, 2013
 *      Author: agrippa
 */
#ifndef MESH_HPP_
#define MESH_HPP_
#include <vector>
#include "types.hpp"
#include "geometry.hpp"
#include "texture.hpp"
namespace ray {
struct TrimeshFace {
    TrimeshFace();
    TrimeshFace(const TrimeshFace& face);
    TrimeshFace(int i, int j,  int k);
    int vertices[3];
};

class Trimesh {
public:
    const std::vector<TrimeshFace>& faces() const;
    int num_faces() const;
    int num_vertices() const;
    const std::vector<glm::vec3>& vertices() const;
    void AddVertex(const glm::vec3& vertex);
    void AddNormal(const glm::vec3& vertex);
    void AddFace(const TrimeshFace& face);
    void AddTexCoord(const tex::TexCoord& tex_coord);
    Triangle GetPatch(int face_index);
private:
    std::vector<glm::vec3> vertices_;
    std::vector<glm::vec3> normals_;
    std::vector<tex::TexCoord> tex_coords_;
    std::vector<TrimeshFace> faces_;
    int material_index;
};
} // namespace ray
#endif /* MESH_HPP_ */
