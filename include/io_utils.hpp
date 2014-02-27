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
#include <stdint.h>
#include <glm/glm.hpp>
namespace ray {
std::ostream& operator <<(std::ostream& out, const glm::vec2& v);
std::ostream& operator<<(std::ostream& out, const glm::vec3& v);
std::ostream& operator<<(std::ostream& out, const glm::vec4& v);
std::ostream& operator<<(std::ostream& out, const glm::mat4x4& m);
template<typename T>
void PrintVector(std::ostream& out, const std::vector<T>& vec,
    const std::string& sep) {
  typename std::vector<T>::const_iterator iter = vec.begin();
  for (; iter != vec.end(); ++iter) {
    if (iter != vec.begin())
      out << sep;
    out << *iter;
  }
}
template<typename T>
void PrintArray(std::ostream& out, T* a, int n, const std::string& sep) {
  for (int i = 0; i < n; ++i) {
    if (i > 0)
      out << sep;
    out << a[i];
  }
}
template<typename T>
void PrintBinary(T c) {
  uint32_t num_bits = sizeof(T) * 8;
  for (uint32_t i = 0; i < num_bits; ++i) {
    if (0 == i % 8 && i > 0)
      std::cout << ' ';
    std::cout << (c & (0x1 << (num_bits - i - 1)) ? '1' : '0');

  }
}
} // namespace ray
#endif /* IO_UTILS_HPP_ */
