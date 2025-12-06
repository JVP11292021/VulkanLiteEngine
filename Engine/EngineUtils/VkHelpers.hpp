#ifndef VLE_HELPERS_H
#define VLE_HELPERS_H

#if defined(USE_VULKAN)

#include <vulkan/vulkan.h>

#if defined(VULKAN_H_)

#include "UtilsDefs.hpp"

#include <vector>
#include <cinttypes>

VLE_UTILS_NS_B

class VertexAttributeBuilder {
public:
    VertexAttributeBuilder(std::uint32_t binding = 0)
        : binding(binding), locationCounter(0) {
    }

    template<typename VertexType, typename MemberType>
    VertexAttributeBuilder& add(VkFormat format, MemberType VertexType::* member) {
        VkVertexInputAttributeDescription desc{};
        desc.binding = binding;
        desc.format = format;
        desc.location = locationCounter++;
        desc.offset = static_cast<std::uint32_t>(memberOffset(member));
        attributes.push_back(desc);
        return *this;
    }

    inline std::vector<VkVertexInputAttributeDescription> build() const {
        return attributes;
    }

private:
    std::uint32_t binding;
    std::uint32_t locationCounter;
    std::vector<VkVertexInputAttributeDescription> attributes;

    template<typename VertexType, typename MemberType>
    static constexpr std::size_t memberOffset(MemberType VertexType::* member) {
        VertexType temp{};
        const std::byte* base = reinterpret_cast<const std::byte*>(&temp);
        const std::byte* ptr = reinterpret_cast<const std::byte*>(&(temp.*member));
        return static_cast<std::size_t>(ptr - base);
    }
};

VLE_UTILS_NS_E

#endif // VULKAN_H
#endif // USE_VULKAN 

#endif // VLE_HELPERS_H