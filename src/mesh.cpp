/*
 * mesh_utils.cpp
 *
 *  Created on: Oct 1, 2013
 *      Author: agrippa
 */
#include <cassert>
#include <cstring>
#include <algorithm>
#include "geometry.hpp"
#include "io_utils.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "shape.hpp"
#include "types.hpp"
namespace ray {
TrimeshFace::TrimeshFace() :
    SceneShape(), mesh_(NULL) {
}

TrimeshFace::TrimeshFace(const TrimeshFace& face) :
    SceneShape(), mesh_(face.mesh_) {
  for (int i = 0; i < 3; ++i) {
    vertices_[i] = face.vertices_[i];
  }
}

TrimeshFace::TrimeshFace(Trimesh* mesh, int i, int j, int k) :
    SceneShape(), mesh_(mesh) {
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

const Trimesh* TrimeshFace::mesh() const {
  return mesh_;
}

void TrimeshFace::set_mesh(Trimesh* mesh) {
  mesh_ = mesh;
}

bool TrimeshFace::Intersect(const Ray& ray, Isect& isect) const {
  bool hit = mesh_ != NULL && mesh_->GetPatch(*this).Intersect(ray, isect);
  if (hit) {
    isect.obj = static_cast<const Shape*>(this);
    isect.normal = mesh_->InterpolateNormal(*this, isect.bary);
    isect.mat = material_;
  }
  return hit;
}

BoundingBox TrimeshFace::GetBounds() const {
  return (mesh_ != NULL ? mesh_->GetPatch(*this).GetBounds() : BoundingBox());
}

const int* TrimeshFace::vertices() const {
  return vertices_;
}

Trimesh::Trimesh() :
    SceneShape(), vertices_(), normals_(), tex_coords_(), faces_(), bounds_(),
        accelerator_(NULL) {
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

void Trimesh::AddFace(int i, int j, int k) {
  TrimeshFace face(this, i, j, k);
  faces_.push_back(face);
  bounds_ = bounds_.Join(face.GetBounds());
}

void Trimesh::AddTexCoord(const TexCoord& tex_coord) {
  tex_coords_.push_back(tex_coord);
}

Triangle Trimesh::GetPatch(const TrimeshFace& face) const {
  Triangle result = Triangle(vertices_[face[0]], vertices_[face[1]],
      vertices_[face[2]]);
  return result;
}

Triangle Trimesh::GetPatch(int face_index) const {
  return GetPatch(faces_[face_index]);
}

glm::vec3 Trimesh::InterpolateNormal(const TrimeshFace& face,
    const glm::vec3& bary) const {
  glm::vec3 N = glm::vec3(0.0f);
  for (int j = 0; j < 3; ++j) {
    N += normals_[face[j]] * bary[j];
  }
  return glm::normalize(N);
}

glm::vec3 Trimesh::InterpolateNormal(int i, const glm::vec3& bary) const {
  return InterpolateNormal(faces_[i], bary);
}

bool Trimesh::IntersectAccelerated(const Ray& ray, Isect& isect) const {
  // [Ray o = [-13.7738,1.08912,1.7613], d = [0.987114,0.0204824,-0.158701]]
  Ray test(glm::vec3(-13.7738f, 1.08912f, 1.7613f),
      glm::vec3(0.987114f, 0.0204824f, -0.158701f));
  bool eq = glm::length(test.origin() - ray.origin()) < 1e-5
      && glm::length(test.direction() - ray.direction()) < 1e-5;
  if (eq)
    accelerator_->set_trace(true);
  bool hit = accelerator_->Intersect(ray, isect);
  if (eq)
    accelerator_->set_trace(false);
  return hit;
}

bool Trimesh::IntersectUnaccelerated(const Ray& ray, Isect& isect) const {
  bool hit = false;
  Isect current, best;
  best.t_hit = std::numeric_limits<float>::max();
  for (uint32_t i = 0; i < faces_.size(); ++i) {
    if (faces_[i].Intersect(ray, current) && current.t_hit < best.t_hit) {
      best = current;
      hit = true;
    }
  }
  if (hit)
    isect = best;
  return hit;
}

bool Trimesh::Intersect(const Ray& ray, Isect& isect) const {
  bool hit = false;
  if (accelerator_)
    hit = IntersectAccelerated(ray, isect);
  else
    hit = IntersectUnaccelerated(ray, isect);
  /**if (accelerator_) {
   Isect isect2;
   bool hit2 = IntersectUnaccelerated(ray, isect2);
   if (hit != hit2 || !(isect == isect2)) {
   std::cout << "\nIntersect Mismatch\n";
   std::cout << "hit = " << hit << "\nhit2 = " << hit2;
   std::cout << "\nisect = " << isect << "\nisect2 = " << isect2;
   accelerator_->set_trace(true);
   IntersectAccelerated(ray, isect);
   }
   assert(hit == hit2);
   assert(isect2 == isect);
   }**/
  if (hit && !isect.mat)
    isect.mat = material_;
  return hit;
}

void Trimesh::GenNormals() {
  normals_.clear();
  normals_.resize(vertices_.size(), glm::vec3(0.0f));
  for (uint32_t i = 0; i < faces_.size(); ++i) {
    glm::vec3 N = GetPatch(i).GetNormal();
    TrimeshFace f = faces_[i];
    for (uint32_t j = 0; j < 3; ++j) {
      normals_[f[j]] += N;
    }
  }
  for (uint32_t i = 0; i < normals_.size(); ++i) {
    normals_[i] = glm::normalize(normals_[i]);
  }
}

BoundingBox Trimesh::GetBounds() {
  return bounds_;
}

void Trimesh::Print(std::ostream& out) const {
  out << "[Trimesh, ";
  out << " v:[";
  PrintVector(out, vertices_, ",");
  out << "]\n";
  out << " n:[";
  PrintVector(out, normals_, ",");
  out << "]\n";
  out << " f:[";
  PrintVector(out, faces_, ",");
  out << "]";
}

void Trimesh::set_accelerator(Accelerator* accelerator) {
  accelerator_ = accelerator;
}

Accelerator* const & Trimesh::accelerator() const {
  return accelerator_;
}
} // namespace ray
