#ifndef VLE_DEVICE_H
#define VLE_DEVICE_H

#include <string>
#include <vector>

#include "defs.hpp"
#if VLE_WIN_ANDROID
#include <android/asset_manager.h>
#include "AndroidWindow.hpp"
#elif VLE_WIN_WINDOWS
#include "GLFWWindow.hpp"
#endif

VLE_NS_B

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    std::uint32_t graphicsFamily;
    std::uint32_t presentFamily;
    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;
    
    inline bool isComplete() const { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

class EngineDevice {
 public:
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

#if VLE_WIN_WINDOWS
    EngineDevice(GLFWWindow& window);
#elif VLE_WIN_ANDROID
	EngineDevice(AndroidWindow& window, AAssetManager* assetManager);
#else
    static_assert(false, "Unsupported platform for EngineDevice");
#endif
    ~EngineDevice();

    // Not copyable or movable
    EngineDevice(const EngineDevice&) = delete;
    EngineDevice& operator=(const EngineDevice&) = delete;
    EngineDevice(EngineDevice&&) = delete;
    EngineDevice&operator=(EngineDevice&&) = delete;

public:
    VkCommandPool getCommandPool() { return this->_commandPool; }
    VkDevice device() { return this->_device; }
#if VLE_WIN_ANDROID
    AAssetManager* assetManager() { return this->_assetManager; }
#endif
    VkSurfaceKHR surface() { return this->_surface; }
    VkQueue graphicsQueue() { return this->_graphicsQueue; }
    VkQueue presentQueue() { return this->_presentQueue; }

    SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(this->_physicalDevice); }
    std::uint32_t findMemoryType(std::uint32_t typeFilter, VkMemoryPropertyFlags properties);
    QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(this->_physicalDevice); }
    VkFormat findSupportedFormat(
        const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

public:
// Buffer Helper Functions
    void createBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer &buffer,
        VkDeviceMemory &bufferMemory);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void copyBufferToImage(
        VkBuffer buffer, VkImage image, std::uint32_t width, std::uint32_t height, std::uint32_t layerCount);

    void createImageWithInfo(
        const VkImageCreateInfo &imageInfo,
        VkMemoryPropertyFlags properties,
        VkImage &image,
        VkDeviceMemory &imageMemory);

    VkPhysicalDeviceProperties properties;

private:
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();

    // helper functions
    bool isDeviceSuitable(VkPhysicalDevice device);
    std::vector<const char *> getRequiredExtensions();
    bool checkValidationLayerSupport();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void hasGflwRequiredInstanceExtensions();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

private:
#if VLE_WIN_WINDOWS
	GLFWWindow& _window;
#elif VLE_WIN_ANDROID
	AndroidWindow& _window;
    AAssetManager* _assetManager;
#else
    static_assert(false, "Unsupported platform for EngineDevice");
#endif

    VkInstance _instance;
    VkDebugUtilsMessengerEXT _debugMessenger;
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkCommandPool _commandPool;

    VkDevice _device;
    VkSurfaceKHR _surface;
    VkQueue _graphicsQueue;
    VkQueue _presentQueue;

    const std::vector<const char *> _validationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char *> _deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
};

VLE_NS_E

#endif //VLE_DEVICE_H