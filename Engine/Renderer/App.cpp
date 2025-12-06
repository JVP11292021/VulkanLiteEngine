#include <iostream>

#include "Renderer.hpp"
#include "RenderSystem.hpp"
#include "PointLightSystem.hpp"

#include <Device.hpp>
#include <defs.hpp>
#include <Window.hpp>
#include <Model.hpp>
#include <Object.hpp>
#include <Camera.hpp>
#include <HID.hpp>
#include <Buffer.hpp>
#include <Descriptors.hpp>
#include <eutils.hpp>

#include <chrono>
#include <memory>
#include <stdexcept>
#include <array>
#include <vector>

struct GlobalUbo {
	glm::mat4 projection{ 1.f };
	glm::mat4 view{ 1.f };

	glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, 0.2f };
	glm::vec3 lightPosition{ -1.f };
	alignas(16) glm::vec4 lightColor{ 1.f };
};

class FirstApp {
public:
	static constexpr std::int32_t WIDTH = 800;
	static constexpr std::int32_t HEIGHT = 600;

	FirstApp() {
		this->globalPool = vle::DescriptorPool::Builder(this->device)
			.setMaxSets(vle::EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, vle::EngineSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		this->loadObjects();
	}

	~FirstApp() {}

	FirstApp(const FirstApp&) = delete;
	void operator=(const FirstApp&) = delete;

	void run() {
		std::vector<std::unique_ptr<vle::Buffer>> uboBuffers(vle::EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (std::int32_t i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<vle::Buffer>(
				this->device,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = vle::DescriptorSetLayout::Builder(this->device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(vle::EngineSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (std::int32_t i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			vle::DescriptorWriter(*globalSetLayout, *this->globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ this->device, this->renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		PointLightSystem pointLigthSystem{ this->device, this->renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
		vle::Camera camera{};

		auto viewerObject = vle::Object::create();
		vle::KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!this->win.shouldClose()) {
			glfwPollEvents();
			 
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTimeElapsed = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			constexpr auto MAX_FRAME_TIME_ELAPSED = 10000.f;
			frameTimeElapsed = glm::min(frameTimeElapsed, MAX_FRAME_TIME_ELAPSED);

			cameraController.moveInPlainXZ(this->win.getGLFWwindow(), frameTimeElapsed, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			auto aspect = this->renderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 25.f);

			if (auto commandBuffer = this->renderer.beginFrame()) {
				std::int32_t frameIndex = this->renderer.getFrameIndex();
				vle::FrameInfo frameInfo{
					frameIndex,
					frameTimeElapsed,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					this->objects
				};

				// Update Phase
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// Render Phase
				this->renderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLigthSystem.render(frameInfo);
				this->renderer.endSwapChainRenderPass(commandBuffer);
				this->renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(this->device.device());
	}
private:

	void loadObjects() {
		std::shared_ptr<vle::ShaderModel> model =
			vle::ShaderModel::createModelFromFile(this->device, "models/smooth_vase.obj");

		const int rows = 1;        // number of objects on Y axis
		const int cols = 8;        // number of objects on X axis

		const float startX = -1.0f;
		const float startY = 0.5f;
		const float zPos = 2.5f;

		const float spacingX = 0.6f;
		const float spacingY = 0.6f;

		for (int y = 0; y < rows; y++) {
			for (int x = 0; x < cols; x++) {

				auto obj = vle::Object::create();
				obj.model = model;

				obj.transform.translation = {
					startX + x * spacingX,
					startY + y * spacingY,
					zPos
				};

				obj.transform.scale = { 3.f, 1.5f, 3.f };

				this->objects.emplace(obj.getId(), std::move(obj));
			}
		}

		std::shared_ptr<vle::ShaderModel> quadModel =
			vle::ShaderModel::createModelFromFile(this->device, "models/quad.obj");
		auto obj = vle::Object::create();
		obj.model = quadModel;
		obj.transform.translation = {0.f, .5f, 0.f};
		obj.transform.scale = { 3.f, 1.0f, 3.f };
		this->objects.emplace(obj.getId(), std::move(obj));

	}

private:
	vle::EngineWindow win{WIDTH, HEIGHT, "Hello Vulkan"};
	vle::EngineDevice device{ win };
	Renderer renderer{ win, device };

	std::unique_ptr<vle::DescriptorPool> globalPool{};
	vle::ObjectMap objects;
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