#ifndef VLE_BUFFER_H
#define VLE_BUFFER_H

/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */


#include "defs.hpp"
#include "Device.hpp"
#include <vulkan/vulkan.h>

VLE_NS_B

class Buffer{
public:
    Buffer(
        EngineDevice& device,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkDeviceSize minOffsetAlignment = 1);
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

public:
    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void unmap();

    void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    void writeToIndex(void* data, int index);
    VkResult flushIndex(int index);
    VkDescriptorBufferInfo descriptorInfoForIndex(int index);
    VkResult invalidateIndex(int index);

public:
    inline VkBuffer getBuffer() const { return this->_buffer; }
    inline void* getMappedMemory() const { return this->_mapped; }
    inline uint32_t getInstanceCount() const { return this->_instanceCount; }
    inline VkDeviceSize getInstanceSize() const { return this->_instanceSize; }
    inline VkDeviceSize getAlignmentSize() const { return this->_instanceSize; }
    inline VkBufferUsageFlags getUsageFlags() const { return this->_usageFlags; }
    inline VkMemoryPropertyFlags getMemoryPropertyFlags() const { return this->_memoryPropertyFlags; }
    inline VkDeviceSize getBufferSize() const { return this->_bufferSize; }

private:
    static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

    EngineDevice& _device;
    void* _mapped = nullptr;
    VkBuffer _buffer = VK_NULL_HANDLE;
    VkDeviceMemory _memory = VK_NULL_HANDLE;

    VkDeviceSize _bufferSize;
    uint32_t _instanceCount;
    VkDeviceSize _instanceSize;
    VkDeviceSize _alignmentSize;
    VkBufferUsageFlags _usageFlags;
    VkMemoryPropertyFlags _memoryPropertyFlags;
};

VLE_NS_E

#endif // VLE_BUFFER_H