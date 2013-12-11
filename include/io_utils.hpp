/*
 * io_utils.hpp
 *
 *  Created on: Nov 6, 2013
 *      Author: agrippa
 */

#ifndef IO_UTILS_HPP_
#define IO_UTILS_HPP_
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
namespace ray {
std::ostream& operator<<(std::ostream& out, const glm::vec3& v);
std::ostream& operator<<(std::ostream& out, const glm::vec4& v);
std::ostream& operator<<(std::ostream& out, const glm::mat4x4& m);
template<typename T>
void PrintVector(
        std::ostream& out,
        const std::vector<T>& vec,
        const std::string& sep) {
    typename std::vector<T>::const_iterator iter = vec.begin();
    for (; iter != vec.end(); ++iter) {
        if (iter != vec.begin()) {
            out << sep;
        }
        out << *iter;
    }
}
} // namespace ray
#endif /* IO_UTILS_HPP_ */
