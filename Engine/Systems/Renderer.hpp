#ifndef VLE_RENDERER_H
#define VLE_RENDERER_H

#include <iostream>

#include "engdefs.hpp"

#include <Device.hpp>
#include <defs.hpp>
#include <Window.hpp>
#include <SwapChain.hpp>

VLE_SYS_NS_B

class Renderer {
public:
	Renderer(vle::EngineWindow& win, vle::EngineDevice& device);
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

private:
	void freeCommandBuffers();
	void createCommandBuffers();
	void recreateSwapChain();

private:
	vle::EngineWindow& win;
	vle::EngineDevice& device;
	std::unique_ptr<vle::EngineSwapChain> swapChain;
	std::vector<VkCommandBuffer> commandBuffers;

	std::uint32_t currentImageIndex;
	std::int32_t currentFrameIndex;
	bool isFrameStarted;
};

VLE_SYS_NS_E

#endif // VLE_RENDERER_H