#include "Window.hpp"

#include <stdexcept>

VLE_NS_B

EngineWindow::EngineWindow(std::int32_t w, std::int32_t h, std::string t) 
	: _width(w), _height(h), _title(t)
{
	this->init();
}

EngineWindow::~EngineWindow() {
	glfwDestroyWindow(this->_win);
	glfwTerminate();
}

void EngineWindow::init() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	this->_win = glfwCreateWindow(this->_width, this->_height, this->_title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(this->_win, this);
	glfwSetFramebufferSizeCallback(this->_win, frameBufferResizedCb);
}

void EngineWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
	if (glfwCreateWindowSurface(instance, this->_win, nullptr, surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan window surface");
	}
}

void EngineWindow::frameBufferResizedCb(GLFWwindow* window, std::int32_t width, std::int32_t height) {
	auto vleWindow = reinterpret_cast<EngineWindow*>(glfwGetWindowUserPointer(window));
	vleWindow->_frameBufferResized = true;
	vleWindow->_width = width;
	vleWindow->_height = height;
}


VLE_NS_E