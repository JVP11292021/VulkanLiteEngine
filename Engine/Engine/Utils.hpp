#ifndef VLE_UTILS_H
#define VLE_UTILS_H

#include "defs.hpp"

#include <functional>

VLE_NS_B

// from: https://stackoverflow.com/a/57595105
template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
	seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
	(hashCombine(seed, rest), ...);
};

VLE_NS_E

#endif // VLE_UTILS_H