#include <iostream>

#include "Renderer.hpp"
#include "RenderSystem.hpp"

#include <Device.hpp>
#include <defs.hpp>
#include <Window.hpp>
#include <Model.hpp>
#include <Object.hpp>
#include <Camera.hpp>

#include <memory>
#include <stdexcept>
#include <array>
#include <vector>

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
		SimpleRenderSystem simpleRenderSystem{ this->device, this->renderer.getSwapChainRenderPass() }; 
		vle::Camera camera{};

		while (!this->win.shouldClose()) {
			glfwPollEvents();
			auto aspect = this->renderer.getAspectRatio();
			//camera.setOrthoProjection(-aspect, aspect, -1, 1, -1, 1);
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 10.f);

			if (auto commandBuffer = this->renderer.beginFrame()) {
				this->renderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, this->objects, camera);
				this->renderer.endSwapChainRenderPass(commandBuffer);
				this->renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(this->device.device());
	}
private:

	std::unique_ptr<vle::ShaderModel> createCubeModel(vle::EngineDevice& device, glm::vec3 offset) {
		std::vector<vle::ShaderModel::Vertex> vertices{
				// left face (white)
				{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
				{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
				{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
				{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
				{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
				{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

				// right face (yellow)
				{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
				{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
				{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
				{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
				{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
				{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
				            
				// top face (orange, remember y axis points down)
				{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
				{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
				{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
				{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
				{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
				{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

				// bottom face (red)
				{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
				{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
				{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
				{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
				{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
				{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

				// nose face (blue)
				{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
				{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
				{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
				{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
				{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
				{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

				// tail face (green)
				{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
				{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
				{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
				{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
				{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
				{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

		};

		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<vle::ShaderModel>(device, vertices);
	}

	void loadObjects() {
		std::shared_ptr<vle::ShaderModel> model = this->createCubeModel(this->device, { .0f,.0f,.0f });
		auto cube = vle::Object::create();
		cube.model = model;
		cube.transform.translation = { .0f,.0f,2.5f };
		cube.transform.scale = { .5f,.5f,.5f };
		this->objects.push_back(std::move(cube));
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