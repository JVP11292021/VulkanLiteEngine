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

  VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
  VkRenderPass getRenderPass() { return renderPass; }
  VkImageView getImageView(int index) { return swapChainImageViews[index]; }
  size_t imageCount() { return swapChainImages.size(); }
  VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
  VkExtent2D getSwapChainExtent() { return swapChainExtent; }
  uint32_t width() { return swapChainExtent.width; }
  uint32_t height() { return swapChainExtent.height; }

  float extentAspectRatio() {
    return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
  }
  VkFormat findDepthFormat();

  VkResult acquireNextImage(uint32_t *imageIndex);
  VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

  bool compareSwapFormats(const EngineSwapChain& swapChain) const { 
      return swapChain.swapChainDepthFormat == this->swapChainDepthFormat && 
          swapChain.swapChainImageFormat == this->swapChainImageFormat; 
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
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  VkFormat swapChainDepthFormat;

  std::vector<VkFramebuffer> swapChainFramebuffers;
  VkRenderPass renderPass;

  std::vector<VkImage> depthImages;
  std::vector<VkDeviceMemory> depthImageMemorys;
  std::vector<VkImageView> depthImageViews;
  std::vector<VkImage> swapChainImages;
  std::vector<VkImageView> swapChainImageViews;

  EngineDevice &device;
  VkExtent2D windowExtent;

  VkSwapchainKHR swapChain;
  std::shared_ptr<EngineSwapChain> oldSwapChain;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  std::vector<VkFence> imagesInFlight;
  size_t currentFrame = 0;
};

VLE_NS_E

#endif // VLE_SWAPCHAIN_H