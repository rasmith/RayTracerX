/*
 * mesh.hpp
 *
 *  Created on: Oct 1, 2013
 *      Author: agrippa
 */
#ifndef MESH_HPP_
#define MESH_HPP_
#include <vector>
#include "geometry.hpp"
#include "texture.hpp"
#include "types.hpp"
namespace ray {
class TrimeshFace {
public:
    TrimeshFace();
    TrimeshFace(const TrimeshFace& face);
    TrimeshFace(int i, int j, int k);
    int& operator[](int i);
    const int& operator[](int i) const;
private:
    int vertices_[3];
};

class Trimesh {
public:
    Trimesh();
    const std::vector<TrimeshFace>& faces() const;
    int num_faces() const;
    int num_vertices() const;
    const std::vector<glm::vec3>& vertices() const;
    void AddVertex(const glm::vec3& vertex);
    void AddNormal(const glm::vec3& vertex);
    void AddFace(const TrimeshFace& face);
    void AddTexCoord(const TexCoord& tex_coord);
    Triangle GetPatch(int face_index) const;
    int material_index() const;
    void set_material_index(int material_index);
    bool Intersect(const Ray& ray, Isect& isect) const;
private:
    std::vector<glm::vec3> vertices_;
    std::vector<glm::vec3> normals_;
    std::vector<TexCoord> tex_coords_;
    std::vector<TrimeshFace> faces_;
    int material_index_;
};
} // namespace ray
#endif /* MESH_HPP_ */
