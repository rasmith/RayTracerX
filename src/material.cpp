/*
 * material.cpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */
#include <algorithm>
#include <ostream>
#include <string>
#include <vector>
#include <boost/unordered_map.hpp>
#include "io_utils.hpp"
#include "material.hpp"
namespace ray {
Material::Material() :
        kd(0.0f, 0.0f, 0.0f), ks(0.0f, 0.0f, 0.0f), ka(0.0, 0.0, 0.0),
                ke(0.0, 0.0, 0.0), kr(0.0f), kt(0.0f), tr(0.0f), ns(0.0f),
                id(-1) {
}

Material::Material(const Material& m) :
        kd(m.kd), ks(m.ks), ka(m.ka), ke(m.ke), kr(m.kr), kt(m.kt), tr(m.tr),
                ns(m.ns), id(m.id) {
}

MaterialList::MaterialList() :
        last_id(-1), materials(), name_to_id_lookup(), id_to_name_lookup() {
}

MaterialList::MaterialList(const MaterialList& l) :
        last_id(l.last_id), materials(l.materials),
                name_to_id_lookup(l.name_to_id_lookup),
                id_to_name_lookup(l.id_to_name_lookup) {
}

int MaterialList::GetMaterialID(const std::string& name) {
    return name_to_id_lookup[name];
}

const std::string& MaterialList::GetMaterialName(int id) {
    return id_to_name_lookup[id];
}

void MaterialList::AddMaterial(const std::string& name, const Material& m) {
    Material mat(m);
    mat.id = ++last_id;
    id_to_name_lookup[mat.id] = name;
    name_to_id_lookup[name] = mat.id;
    materials.push_back(mat);
}

std::ostream& operator<<(std::ostream& out, const Material& material) {
    out << "[kd:" << material.kd << ",";
    out << "ks:" << material.ks << ",";
    out << "ka:" << material.ka << ",";
    out << "ke:" << material.ke << ",";
    out << "kr:" << material.kr << ",";
    out << "kt:" << material.kt << ",";
    out << "tr:" << material.tr << ",";
    out << "ns:" << material.ns << ",";
    out << "[id:" << material.id << "]";
    return out;
}
std::ostream& operator<<(std::ostream& out, const MaterialList& list) {
    boost::unordered_map<std::string, int>::const_iterator iter = list
            .name_to_id_lookup.begin();
    out << "[";
    for (; iter != list.name_to_id_lookup.end(); ++iter) {
        if (iter != list.name_to_id_lookup.begin())
            out << ",";
        out << "['" << iter->first << "'," << list.materials[iter->second]
                << "]";
    }
    out << "]";
    return out;
}
} // namespace ray
