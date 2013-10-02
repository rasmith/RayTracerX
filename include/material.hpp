/*
 * material.hpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */

#ifndef MATERIAL_HPP_
#define MATERIAL_HPP_
#include <vector>
#include <string>
#include <boost/unordered_map.hpp>
#include "types.hpp"
namespace ray {
namespace mat {
struct Material {
    Material();
    Material(const Material& m);
    glm::vec3 kd;// diffuse components
    glm::vec3 ks;// specular components
    glm::vec3 ka;// ambient components
    glm::vec3 ke;// emissive components
    float tr;    // opacity
    float ns;    // shineness
    int id;      // unique id
};
struct MaterialList {
public:
    MaterialList();
    MaterialList(const MaterialList& l);
    std::vector<Material> materials;
    boost::unordered_map<std::string, int> name_to_id_lookup;
    boost::unordered_map<int, std::string> id_to_name_lookup;
    int GetMaterialID(const std::string& name);
    int GetMaterialName(int id);
    void AddMaterial(const std::string& name, const Material& m);
    int last_id;
};
}
}
#endif /* MATERIAL_HPP_ */
