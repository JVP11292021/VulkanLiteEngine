#include <iostream>

#include "Renderer.hpp"
#include "RenderSystem.hpp"

#include <Device.hpp>
#include <defs.hpp>
#include <Window.hpp>
#include <Model.hpp>
#include <Object.hpp>

#include <memory>
#include <stdexcept>
#include <array>

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

		while (!this->win.shouldClose()) {
			glfwPollEvents();
			if (auto commandBuffer = this->renderer.beginFrame()) {
				this->renderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, this->objects);
				this->renderer.endSwapChainRenderPass(commandBuffer);
				this->renderer.endFrame();
			}
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

	void loadObjects() {
		std::vector<vle::ShaderModel::Vertex> vertices{};
		sierpinski(vertices, 5, { -0.5f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, -0.5f }, { 0.0f, 0.0f, 1.0f });
		std::cout << "Vertices count: " << vertices.size() * 3 << "\n";
		//std::vector<vle::ShaderModel::Vertex> vertices{
		//	{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		//	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		//	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };
		auto model = std::make_shared<vle::ShaderModel>(this->device, vertices);

		auto triangle = vle::Object::create();
		triangle.model = model;
		triangle.color = { .1f, .8f, .1f };
		triangle.transform2d.translation.x = .2f;
		triangle.transform2d.scale = {2.f, .5f};
		triangle.transform2d.rotation = .25f * glm::two_pi<float>();
		this->objects.push_back(std::move(triangle));
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