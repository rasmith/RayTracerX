/*
 * mesh_utils.cpp
 *
 *  Created on: Oct 1, 2013
 *      Author: agrippa
 */
#include <cstring>
#include <algorithm>
#include "geometry.hpp"
#include "mesh.hpp"
#include "types.hpp"
namespace ray {
TrimeshFace::TrimeshFace() :
        mesh_(NULL) {
}

TrimeshFace::TrimeshFace(const TrimeshFace& face) :
        mesh_(face.mesh_) {
    std::copy(vertices_, vertices_ + 3, face.vertices_);
}

TrimeshFace::TrimeshFace(Trimesh* mesh, int i, int j, int k) :
        mesh_(mesh) {
    vertices_[0] = i;
    vertices_[1] = j;
    vertices_[2] = k;
}

int& TrimeshFace::operator[](int i) {
    return vertices_[i];
}

const int& TrimeshFace::operator[](int i) const {
    return vertices_[i];
}

const Trimesh*& TrimeshFace::mesh() const {
    return mesh_;
}

void TrimeshFace::set_mesh(const Trimesh*& mesh) {
    mesh_ = mesh;
}

bool TrimeshFace::Intersect(const Ray& ray, Isect& isect) const {
    return mesh_ != NULL && mesh_->GetPatch(*this).Intersect(ray, isect);
}

BoundingBox TrimeshFace::GetBounds() {
    return (mesh_ != NULL ? mesh_->GetPatch(*this).GetBounds() : BoundingBox());
}

const int* TrimeshFace::vertices() const {
    return vertices_;
}

Trimesh::Trimesh() :
        vertices_(), normals_(), tex_coords_(), faces_(), material_index_(-1) {
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

void Trimesh::AddTexCoord(const TexCoord& tex_coord) {
    tex_coords_.push_back(tex_coord);
}

Triangle Trimesh::GetPatch(const TrimeshFace& face) const {
    return Triangle(vertices_[face[0]], vertices_[face[1]], vertices_[face[2]]);
}

Triangle Trimesh::GetPatch(int face_index) const {
    return GetPatch(faces_[face_index]);
}

int Trimesh::material_index() const {
    return material_index_;
}

void Trimesh::set_material_index(int material_index) {
    material_index_ = material_index;
}
bool Trimesh::Intersect(const Ray& ray, Isect& isect) const {
    bool hit = false;
    Isect current;
    Isect best;
    for (uint32_t i = 0; i < faces_.size(); ++i) {
        Triangle patch = GetPatch(i);
        if (patch.Intersect(ray, current) && current.t_hit < best.t_hit) {
            best = current;
            hit = true;
        }
    }
    isect = best;
    return hit;
}

BoundingBox Trimesh::GetBounds() {
}

} // namespace ray
