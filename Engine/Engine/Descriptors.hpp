#ifndef VLE_DESCRIPTOR_H
#define VLE_DESCRIPTOR_H

#include "defs.hpp"

#include "Device.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

VLE_NS_B

class DescriptorSetLayout {
public:
    class Builder {
    public:
        Builder(EngineDevice& device) : _device{ device } {}

        Builder& addBinding(
            uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stageFlags,
            uint32_t count = 1);
        std::unique_ptr<DescriptorSetLayout> build() const;

    private:
        EngineDevice& _device;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> _bindings{};
    };

    DescriptorSetLayout(
        EngineDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
    ~DescriptorSetLayout();
    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

public:
    inline VkDescriptorSetLayout getDescriptorSetLayout() const { return this->_descriptorSetLayout; }

private:
    EngineDevice& _device;
    VkDescriptorSetLayout _descriptorSetLayout;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> _bindings;

    friend class DescriptorWriter;
};

class DescriptorPool {
public:
    class Builder {
    public:
        Builder(EngineDevice& device) : _device{ device } {}

        Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
        Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
        Builder& setMaxSets(uint32_t count);
        std::unique_ptr<DescriptorPool> build() const;
       
    private:
        EngineDevice& _device;
        std::vector<VkDescriptorPoolSize> _poolSizes{};
        uint32_t _maxSets = 1000;
        VkDescriptorPoolCreateFlags _poolFlags = 0;
    };

public:
    DescriptorPool(
        EngineDevice& device,
        uint32_t maxSets,
        VkDescriptorPoolCreateFlags poolFlags,
        const std::vector<VkDescriptorPoolSize>& poolSizes);
    ~DescriptorPool();
    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;

public:
    bool allocateDescriptor(
        const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

    void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

    void resetPool();

private:
    EngineDevice& _device;
    VkDescriptorPool _descriptorPool;

    friend class DescriptorWriter;
};

class DescriptorWriter {
public:
    DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool);

public:
    DescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
    DescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

    bool build(VkDescriptorSet& set);
    void overwrite(VkDescriptorSet& set);

private:
    DescriptorSetLayout& _setLayout;
    DescriptorPool& _pool;
    std::vector<VkWriteDescriptorSet> _writes;
};

VLE_NS_E

#endif // VLE_DESCRIPTOR_H