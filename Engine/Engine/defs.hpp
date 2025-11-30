#ifndef VLE_DEFS_H
#define VLE_DEFS_H

#include <cstdlib>
#include <cinttypes>
#include <exception>
#include <cassert>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#define VLE_NS_B namespace vle {
#define VLE_NS_E }

#define VLE_PAD_TO_ALIGNMENT(T, N) \
    alignas(N) char _padding[(N - (sizeof(T) % N)) % N]


#endif // VLE_DEFS_H