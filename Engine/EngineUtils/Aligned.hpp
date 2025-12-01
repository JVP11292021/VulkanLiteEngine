#ifndef VLE_UTILS_ALIGNMENT_H
#define VLE_UTILS_ALIGNMENT_H

#include "UtilsDefs.hpp"

#include <cstddef>

VLE_UTILS_NS_B

#define VLE_PAD_TO_ALIGNMENT(T, N) \
    alignas(N) char _padding[(N - (sizeof(T) % N)) % N]

template <typename T, std::size_t Alignment>
struct AlignedBlock {
    alignas(Alignment) T data;

private:
    static constexpr std::size_t base = sizeof(T);
    static constexpr std::size_t remainder = base % Alignment;
    static constexpr std::size_t padding = (remainder == 0 ? 0 : Alignment - remainder);

    char _padding[padding]{};
};

VLE_UTILS_NS_E


#endif // VLE_UTILS_ALIGNMENT_H