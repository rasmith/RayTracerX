/*
 * mesh_utils.hpp
 *
 *  Created on: Oct 1, 2013
 *      Author: agrippa
 */

#ifndef MESH_UTILS_HPP_
#define MESH_UTILS_HPP_

#include <vector>
#include "types.hpp"

namespace ray {
namespace mesh {

struct TriFace {
    int vertices[3];
};

class TriMesh {
public:
    const std::vector<TriFace>& faces() const;
    void set_faces(const std::vector<TriFace>& faces);
    int num_faces() const;
    void set_num_faces(int num_faces);
    int num_vertices() const;
    void set_num_vertices(int num_vertices);
    const std::vector<glm::vec3>& vertices() const;
    void set_vertices(const std::vector<glm::vec3>& vertices);
private:
    int num_vertices_;
    std::vector<glm::vec3> vertices_;
    std::vector<glm::vec3> normals_;
    int num_faces_;
    std::vector<TriFace> faces_;
    std::vector<int> materials_;
};

}
}

#endif /* MESH_UTILS_HPP_ */
