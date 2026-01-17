#ifndef VLE_RENDERER_H
#define VLE_RENDERER_H

#include <iostream>

#include "engdefs.hpp"

#include <defs.hpp>
#include <Device.hpp>
#include <GLFWWindow.hpp>
#include <AndroidWindow.hpp>
#include <SwapChain.hpp>

VLE_SYS_NS_B

class Renderer {
public:
#if VLE_WIN_WINDOWS
	Renderer(vle::GLFWWindow& win, vle::EngineDevice& device);
#elif VLE_WIN_ANDROID
	Renderer(vle::AndroidWindow& win, vle::EngineDevice& device);
#else
	throw std::runtime_error("Unsupported platform for Renderer");
#endif

	~Renderer();

	Renderer(const Renderer&) = delete;
	void operator=(const Renderer&) = delete;

	VkRenderPass getSwapChainRenderPass() const { return this->swapChain->getRenderPass(); }
	inline float getAspectRatio() const { return this->swapChain->extentAspectRatio(); }
	inline bool isFrameInProgress() const { return this->isFrameStarted; }
	inline VkCommandBuffer getCurrentCommandBuffer() const { 
		assert(this->isFrameStarted && "Could not get command buffer when frame is not in progress");
		return this->commandBuffers[this->currentFrameIndex]; 
	}
	inline std::int32_t getFrameIndex() const {
		assert(this->isFrameStarted && "Cannot get frameIndex when frame is not in progress");
		return this->currentFrameIndex;
	}

public:
	VkCommandBuffer beginFrame();
	void endFrame();
	void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
	void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
	void recreateSwapChain();

private:
	void freeCommandBuffers();
	void createCommandBuffers();

private:
#if VLE_WIN_WINDOWS
	vle::GLFWWindow& win;
#elif VLE_WIN_ANDROID
	vle::AndroidWindow& win;
#else
	throw std::runtime_error("Unsupported platform for Renderer");
#endif

	vle::EngineDevice& device;
	std::unique_ptr<vle::EngineSwapChain> swapChain;
	std::vector<VkCommandBuffer> commandBuffers;

	std::uint32_t currentImageIndex;
	std::int32_t currentFrameIndex;
	bool isFrameStarted;
};

VLE_SYS_NS_E

#endif // VLE_RENDERER_H