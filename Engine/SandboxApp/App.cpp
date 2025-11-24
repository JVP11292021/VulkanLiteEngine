#include <iostream>

#include "Device.hpp"
#include "defs.hpp"
#include "Window.hpp"
#include "SwapChain.hpp"
#include "Pipeline.hpp"
#include "Model.hpp"

#include <memory>
#include <stdexcept>
#include <array>

class FirstApp {
public:
	static constexpr std::int32_t WIDTH = 800;
	static constexpr std::int32_t HEIGHT = 600;

	FirstApp() {
		this->loadModels();
		this->createPipelineLayout();
		this->recreateSwapChain();
		this->createCommandBuffers();
	}

	~FirstApp() {
		vkDestroyPipelineLayout(this->device.device(), pipelineLayout, nullptr);
	}

	FirstApp(const FirstApp&) = delete;
	void operator=(const FirstApp&) = delete;

	void run() {
		while (!this->win.shouldClose()) {
			glfwPollEvents();
			this->drawFrame();
		}

		vkDeviceWaitIdle(this->device.device());
	}
private:
	void sierpinski(
		std::vector<vle::ShaderModel::Vertex>& vertices,
		int depth,
		glm::vec2 left,
		glm::vec3 leftColor,
		glm::vec2 right,
		glm::vec3 rightColor,
		glm::vec2 top,
		glm::vec3 topColor
	) {
		if (depth <= 0) {
			vertices.push_back({ top, topColor });
			vertices.push_back({ right, rightColor });
			vertices.push_back({ left, leftColor });
		}
		else {
			glm::vec2 leftTop = 0.5f * (left + top);
			glm::vec2 rightTop = 0.5f * (right + top);
			glm::vec2 leftRight = 0.5f * (left + right);
			glm::vec3 leftTopColor = 0.5f * (leftColor + topColor);
			glm::vec3 rightTopColor = 0.5f * (rightColor + topColor);
			glm::vec3 leftRightColor = 0.5f * (leftColor + rightColor);
			sierpinski(vertices, depth - 1, left, leftColor, leftRight, leftRightColor, leftTop, topColor);
			sierpinski(vertices, depth - 1, leftRight, leftRightColor, right, rightColor, rightTop, rightTopColor);
			sierpinski(vertices, depth - 1, leftTop, leftTopColor, rightTop, rightTopColor, top, topColor);
		}
	}

	void loadModels() {
		std::vector<vle::ShaderModel::Vertex> vertices{};
		sierpinski(vertices, 5, { -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, -0.5f }, { 0.0f, 0.0f, 1.0f });
		this->model = std::make_unique<vle::ShaderModel>(this->device, vertices);
	}

	void createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(this->device.device(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void createPipeline() {
		assert(this->swapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		vle::PipelineConfigInfo pipelineConfig{};
		vle::Pipeline::defaultPipelineConfigInfo(
			pipelineConfig);
		pipelineConfig.renderPass = this->swapChain->getRenderPass();
		pipelineConfig.renderPass = this->swapChain->getRenderPass();
		pipelineConfig.pipelineLayout = this->pipelineLayout;
		this->pipeline = std::make_unique<vle::Pipeline>(device, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
	}

	void freeCommandBuffers() {
		vkFreeCommandBuffers(
			this->device.device(),
			this->device.getCommandPool(),
			static_cast<std::uint32_t>(this->commandBuffers.size()),
			this->commandBuffers.data());
		this->commandBuffers.clear();
	}

	void createCommandBuffers() {
		this->commandBuffers.resize(this->swapChain->imageCount());
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = this->device.getCommandPool();
		allocInfo.commandBufferCount = static_cast<std::uint32_t>(this->commandBuffers.size());

		if (vkAllocateCommandBuffers(this->device.device(), &allocInfo, this->commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void drawFrame() {
		std::uint32_t imageIndex;
		auto result = this->swapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			this->recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		this->recordCommandBuffer(imageIndex);
		result = this->swapChain->submitCommandBuffers(&this->commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || this->win.wasWindowResized()) {
			this->win.resetWindowResized();
			this->recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image");
		}
	}

	void recreateSwapChain() {
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
			this->swapChain= std::make_unique<vle::EngineSwapChain>(this->device, extent, oldSwapChain);
			assert(
				this->swapChain->imageCount() == oldSwapChain->imageCount() &&
				"Swap chain image count has changed!");
		}

		this->createPipeline();
	}

	void recordCommandBuffer(std::int32_t imageIndex) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(this->commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = this->swapChain->getRenderPass();
		renderPassInfo.framebuffer = this->swapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = this->swapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<std::uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(this->commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(this->swapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(this->swapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, this->swapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		this->pipeline->bind(this->commandBuffers[imageIndex]);
		this->model->bind(this->commandBuffers[imageIndex]);
		this->model->draw(this->commandBuffers[imageIndex]);

		vkCmdEndRenderPass(this->commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(this->commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
		}
	}

private:
	vle::EngineWindow win{WIDTH, HEIGHT, "Hello Vulkan"};
	vle::EngineDevice device{ win };
	std::unique_ptr<vle::EngineSwapChain> swapChain;
	std::unique_ptr<vle::Pipeline> pipeline;
	VkPipelineLayout pipelineLayout;
	std::vector<VkCommandBuffer> commandBuffers;
	std::unique_ptr<vle::ShaderModel> model;
};

int main() {
	FirstApp app;
	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}


