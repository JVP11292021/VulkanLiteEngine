#include <iostream>

#include "Renderer.hpp"
#include "RenderSystem.hpp"

#include <Device.hpp>
#include <defs.hpp>
#include <Window.hpp>
#include <Model.hpp>
#include <Object.hpp>
#include <Camera.hpp>
#include <HID.hpp>
#include <Buffer.hpp>

#include <chrono>
#include <memory>
#include <stdexcept>
#include <array>
#include <vector>

struct GlobalUbo {
	glm::mat4 projectionView{ 1.f };
	glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f,-3.f,-2.f });
};

class FirstApp {
public:
	static constexpr std::int32_t WIDTH = 800;
	static constexpr std::int32_t HEIGHT = 600;

	FirstApp() {
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

		SimpleRenderSystem simpleRenderSystem{ this->device, this->renderer.getSwapChainRenderPass() }; 
		vle::Camera camera{};
		camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(.0f, .5f, 2.5f));

		auto viewerObject = vle::Object::create();
		vle::KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!this->win.shouldClose()) {
			glfwPollEvents();
			 
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTimeElapsed = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();

			constexpr auto MAX_FRAME_TIME_ELAPSED = 10000.f;
			frameTimeElapsed = glm::min(frameTimeElapsed, MAX_FRAME_TIME_ELAPSED);

			cameraController.moveInPlainXZ(this->win.getGLFWwindow(), frameTimeElapsed, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			auto aspect = this->renderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 10.f);

			if (auto commandBuffer = this->renderer.beginFrame()) {
				std::int32_t frameIndex = this->renderer.getFrameIndex();
				vle::FrameInfo frameInfo{
					frameIndex,
					frameTimeElapsed,
					commandBuffer,
					camera
				};

				// Update Phase
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo, frameIndex);
				uboBuffers[frameIndex]->flushIndex(frameIndex);

				// Render Phase
				this->renderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo, this->objects);
				this->renderer.endSwapChainRenderPass(commandBuffer);
				this->renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(this->device.device());
	}
private:

	void loadObjects() {
		std::shared_ptr<vle::ShaderModel> model = vle::ShaderModel::createModelFromFile(this->device, "models/flat_vase.obj");
		auto centreObject = vle::Object::create();
		centreObject.model = model;
		centreObject.transform.translation = { .0f,.0f,2.5f };
		centreObject.transform.scale = glm::vec3{ 3.f };
		this->objects.push_back(std::move(centreObject));
	}

private:
	vle::EngineWindow win{WIDTH, HEIGHT, "Hello Vulkan"};
	vle::EngineDevice device{ win };
	Renderer renderer{ win, device };

	std::vector<vle::Object> objects;
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