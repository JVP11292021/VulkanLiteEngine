#ifndef VLE_SWAPCHAIN_H
#define VLE_SWAPCHAIN_H

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <memory>

#include "Device.hpp"
#include "defs.hpp"

VLE_NS_B

class EngineSwapChain {
public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    EngineSwapChain(EngineDevice &deviceRef, VkExtent2D windowExtent);
    EngineSwapChain(
        EngineDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<EngineSwapChain> previous);
    ~EngineSwapChain();

    EngineSwapChain(const EngineSwapChain&) = delete;
    EngineSwapChain& operator=(const EngineSwapChain&) = delete;

    VkFramebuffer getFrameBuffer(int index) { return this->_swapChainFramebuffers[index]; }
    VkRenderPass getRenderPass() { return this->_renderPass; }
    VkImageView getImageView(int index) { return this->_swapChainImageViews[index]; }
    size_t imageCount() { return this->_swapChainImages.size(); }
    VkFormat getSwapChainImageFormat() { return this->_swapChainImageFormat; }
    VkExtent2D getSwapChainExtent() { return this->_swapChainExtent; }
    uint32_t width() { return this->_swapChainExtent.width; }
    uint32_t height() { return this->_swapChainExtent.height; }

    inline float extentAspectRatio() {
        return static_cast<float>(this->_swapChainExtent.width) / static_cast<float>(this->_swapChainExtent.height);
    }
    VkFormat findDepthFormat();

    VkResult acquireNextImage(uint32_t *imageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

    inline bool compareSwapFormats(const EngineSwapChain& swapChain) const { 
        return swapChain._swapChainDepthFormat == this->_swapChainDepthFormat && 
            swapChain._swapChainImageFormat == this->_swapChainImageFormat; 
    }

private:
    void init();
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    // Helper functions
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

private:
    VkFormat _swapChainImageFormat;
    VkExtent2D _swapChainExtent;
    VkFormat _swapChainDepthFormat;

    std::vector<VkFramebuffer> _swapChainFramebuffers;
    VkRenderPass _renderPass;

    std::vector<VkImage> _depthImages;
    std::vector<VkDeviceMemory> _depthImageMemorys;
    std::vector<VkImageView> _depthImageViews;
    std::vector<VkImage> _swapChainImages;
    std::vector<VkImageView> _swapChainImageViews;

    EngineDevice &_device;
    VkExtent2D _windowExtent;

    VkSwapchainKHR _swapChain;
    std::shared_ptr<EngineSwapChain> _oldSwapChain;

    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    std::vector<VkFence> _imagesInFlight;
    size_t _currentFrame = 0;
};

VLE_NS_E

#endif // VLE_SWAPCHAIN_H