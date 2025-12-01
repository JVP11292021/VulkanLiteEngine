#ifndef VLE_UTILS_HASH_H
#define VLE_UTILS_HASH_H

#include "UtilsDefs.hpp"

#include <functional>

VLE_UTILS_NS_B

// from: https://stackoverflow.com/a/57595105
template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
	seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
	(hashCombine(seed, rest), ...);
};

VLE_UTILS_NS_E

#endif // VLE_UTILS_HASH_H