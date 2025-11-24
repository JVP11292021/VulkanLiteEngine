//#include "Renderer.hpp"
//
//VkCommandBuffer Renderer::beginFrame() {
//
//}
//
//void Renderer::endFrame() {
//
//}
//
//void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
//
//}
//
//void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
//
//}
//
//void Renderer::freeCommandBuffers() {
//	vkFreeCommandBuffers(
//		this->device.device(),
//		this->device.getCommandPool(),
//		static_cast<std::uint32_t>(this->commandBuffers.size()),
//		this->commandBuffers.data());
//	this->commandBuffers.clear();
//}
//
//void Renderer::createCommandBuffers() {
//	this->commandBuffers.resize(this->swapChain->imageCount());
//	VkCommandBufferAllocateInfo allocInfo{};
//	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//	allocInfo.commandPool = this->device.getCommandPool();
//	allocInfo.commandBufferCount = static_cast<std::uint32_t>(this->commandBuffers.size());
//
//	if (vkAllocateCommandBuffers(this->device.device(), &allocInfo, this->commandBuffers.data()) != VK_SUCCESS) {
//		throw std::runtime_error("Failed to allocate command buffers");
//	}
//}
//
//void drawFrame() {
//	std::uint32_t imageIndex;
//	auto result = this->swapChain->acquireNextImage(&imageIndex);
//
//	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
//		this->recreateSwapChain();
//		return;
//	}
//
//	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
//		throw std::runtime_error("Failed to acquire swap chain image");
//	}
//
//	this->recordCommandBuffer(imageIndex);
//	result = this->swapChain->submitCommandBuffers(&this->commandBuffers[imageIndex], &imageIndex);
//	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || this->win.wasWindowResized()) {
//		this->win.resetWindowResized();
//		this->recreateSwapChain();
//		return;
//	}
//	if (result != VK_SUCCESS) {
//		throw std::runtime_error("Failed to present swap chain image");
//	}
//}
//
//void Renderer::recreateSwapChain() {
//	auto extent = this->win.getExtent();
//	while (extent.width == 0 || extent.height == 0) {
//		extent = this->win.getExtent();
//		glfwWaitEvents();
//	}
//	vkDeviceWaitIdle(this->device.device());
//
//	if (this->swapChain == nullptr) {
//		this->swapChain = std::make_unique<vle::EngineSwapChain>(this->device, extent);
//	}
//	else {
//		std::shared_ptr<vle::EngineSwapChain> oldSwapChain = std::move(this->swapChain);
//		this->swapChain = std::make_unique<vle::EngineSwapChain>(this->device, extent, oldSwapChain);
//		assert(
//			this->swapChain->imageCount() == oldSwapChain->imageCount() &&
//			"Swap chain image count has changed!");
//	}
//
//	this->createPipeline();
//}