/*
 * mesh_utils.cpp
 *
 *  Created on: Oct 1, 2013
 *      Author: agrippa
 */
#include <cstring>
#include "types.hpp"
#include "mesh.hpp"
#include "geometry.hpp"

namespace ray {
namespace mesh {

TrimeshFace::TrimeshFace() {
}

TrimeshFace::TrimeshFace(const TrimeshFace& face) {
    memcpy(vertices, face.vertices, sizeof(int) * 3);
}

TrimeshFace::TrimeshFace(int i, int j, int k) {
    vertices[0] = i;
    vertices[1] = j;
    vertices[2] = k;
}

TexCoord::TexCoord() {
}

TexCoord::TexCoord(const TexCoord& texcoord) :
        coords(texcoord.coords) {
}

TexCoord::TexCoord(const glm::vec3& values) :
        coords(values) {
}

const std::vector<TrimeshFace>& Trimesh::faces() const {
    return faces_;
}

int Trimesh::num_faces() const {
    return faces_.size();
}

int Trimesh::num_vertices() const {
    return vertices_.size();
}

const std::vector<glm::vec3>& Trimesh::vertices() const {
    return vertices_;
}

void Trimesh::AddVertex(const glm::vec3& vertex) {
    vertices_.push_back(vertex);
}

void Trimesh::AddNormal(const glm::vec3& normal) {
    normals_.push_back(normal);
}

void Trimesh::AddFace(const TrimeshFace& face) {
    faces_.push_back(face);
}

geom::Triangle Trimesh::GetPatch(int face_index) {
    TrimeshFace face = faces_[face_index];
    return geom::Triangle(vertices_[face.vertices[0]], vertices_[face.vertices[1]],
            vertices_[face.vertices[0]]);
}

}
}
