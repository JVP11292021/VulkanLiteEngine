#include "Renderer.hpp"

#include <array>

VLE_SYS_NS_B

Renderer::Renderer(vle::EngineWindow& win, vle::EngineDevice& device) 
	: win(win), device(device), currentImageIndex(0), currentFrameIndex(0), isFrameStarted(false)
{
	this->recreateSwapChain();
	this->createCommandBuffers();
}

Renderer::~Renderer() {
	this->freeCommandBuffers();
}

VkCommandBuffer Renderer::beginFrame() {
	assert(!isFrameStarted && "Can't call beginFrame while in progress");
	auto result = this->swapChain->acquireNextImage(&this->currentImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		this->recreateSwapChain();
		return nullptr;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swap chain image");
	}

	this->isFrameStarted = true;

	auto commandBuffer = this->getCurrentCommandBuffer();

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer");
	}

	return commandBuffer;
}

void Renderer::endFrame() {
	assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
	auto commandBuffer = this->getCurrentCommandBuffer();
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer");
	}

	auto result = this->swapChain->submitCommandBuffers(&commandBuffer, &this->currentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || this->win.wasWindowResized()) {
		this->win.resetWindowResized();
		this->recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swap chain image");
	}

	this->isFrameStarted = false;
	this->currentFrameIndex = (this->currentFrameIndex + 1) % vle::EngineSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
	assert(isFrameStarted && "Can't call beginSwapChainRenderPass while frame is not in progress");
	assert(commandBuffer == this->getCurrentCommandBuffer() && "Can't call beginSwapChainRenderPass from a different frame");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = this->swapChain->getRenderPass();
	renderPassInfo.framebuffer = this->swapChain->getFrameBuffer(this->currentImageIndex);

	renderPassInfo.renderArea.offset = { 0,0 };
	renderPassInfo.renderArea.extent = this->swapChain->getSwapChainExtent();

	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<std::uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(this->swapChain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(this->swapChain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0, 0}, this->swapChain->getSwapChainExtent() };
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
	assert(isFrameStarted && "Can't call endSwapChainRenderPass while frame is not in progress");
	assert(commandBuffer == this->getCurrentCommandBuffer() && "Can't call endSwapChainRenderPass from a different frame");
	vkCmdEndRenderPass(commandBuffer);
}

void Renderer::freeCommandBuffers() {
	vkFreeCommandBuffers(
		this->device.device(),
		this->device.getCommandPool(),
		static_cast<std::uint32_t>(this->commandBuffers.size()),
		this->commandBuffers.data());
	this->commandBuffers.clear();
}

void Renderer::createCommandBuffers() {
	this->commandBuffers.resize(vle::EngineSwapChain::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = this->device.getCommandPool();
	allocInfo.commandBufferCount = static_cast<std::uint32_t>(this->commandBuffers.size());

	if (vkAllocateCommandBuffers(this->device.device(), &allocInfo, this->commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffers");
	}
}

void Renderer::recreateSwapChain() {
	auto extent = this->win.getExtent();
	while (extent.width == 0 || extent.height == 0) {
		extent = this->win.getExtent();
		glfwWaitEvents();
	}
	vkDeviceWaitIdle(this->device.device());

	if (this->swapChain == nullptr) {
		this->swapChain = std::make_unique<vle::EngineSwapChain>(this->device, extent);
	}
	else {
		std::shared_ptr<vle::EngineSwapChain> oldSwapChain = std::move(this->swapChain);
		this->swapChain = std::make_unique<vle::EngineSwapChain>(this->device, extent, oldSwapChain);
		assert(
			this->swapChain->imageCount() == oldSwapChain->imageCount() &&
			"Swap chain image count has changed!");

		if (!oldSwapChain->compareSwapFormats(*this->swapChain.get())) {
			throw std::runtime_error("Swap chain image(or depth) format ahs changed");
		}
	
	}
}

VLE_SYS_NS_E