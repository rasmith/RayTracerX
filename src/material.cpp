/*
 * material.cpp
 *
 *  Created on: Oct 2, 2013
 *      Author: agrippa
 */
#include <vector>
#include <boost/unordered_map.hpp>
#include <algorithm>
#include "material.hpp"
namespace ray {
namespace mat {
Material::Material() :
        kd(0.0f, 0.0f, 0.0f), ks(0.0f, 0.0f, 0.0f), ka(0.0, 0.0, 0.0),
                ke(0.0, 0.0, 0.0), tr(0.0f), ns(0.0f), id(-1) {
}

Material::Material(const Material& m) :
        kd(m.kd), ks(m.ks), ka(m.ka), ke(m.ke), tr(m.tr), ns(m.ns), id(m.id) {
}

MaterialList::MaterialList() :
        last_id(-1) {
    materials.clear();
    name_to_id_lookup.clear();
    id_to_name_lookup.clear();
}

MaterialList::MaterialList(const MaterialList& l) :
        last_id(l.last_id) {
    std::copy(l.materials.begin(), l.materials.end(), materials.begin());
    std::copy(l.name_to_id_lookup.begin(), l.name_to_id_lookup.end(),
            name_to_id_lookup.begin());
    std::copy(l.id_to_name_lookup.begin(), l.id_to_name_lookup.end(),
            id_to_name_lookup.begin());
}

int MaterialList::GetMaterialID(const std::string& name) {
    return name_to_id_lookup[name];
}

int MaterialList::GetMaterialName(int id) {
    return id_to_name_lookup[id];
}

void MaterialList::AddMaterial(const std::string& name,
        const Material& m) {
    m.id = ++last_id;
    id_to_name_lookup[m.id] = name;
    name_to_id_lookup[name] = m.id;
}
} // namespace ray
} // namespace mat
