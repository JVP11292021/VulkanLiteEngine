#ifndef VLE_WINDOW_H
#define VLE_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cinttypes>
#include <string>

#include "defs.hpp"

VLE_NS_B

class EngineWindow {
public:
	 EngineWindow(std::int32_t w, std::int32_t h, std::string t);
	~EngineWindow();

	EngineWindow(const EngineWindow&) = delete;
	EngineWindow& operator=(const EngineWindow&) = delete;

public:
	inline bool shouldClose() { return glfwWindowShouldClose(this->_win); }
	inline VkExtent2D getExtent() const { return { static_cast<std::uint32_t>(this->_width), static_cast<std::uint32_t>(this->_height) }; }
	inline bool wasWindowResized() const { return this->_frameBufferResized; }
	inline void resetWindowResized() { this->_frameBufferResized = false; }

public:
	void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

private:
	static void frameBufferResizedCb(GLFWwindow* window, std::int32_t width, std::int32_t height);
	void init();

private:
	GLFWwindow* _win;

	std::int32_t _width;
	std::int32_t _height;
	bool _frameBufferResized;
	std::string _title;
};

VLE_NS_E

#endif // VLE_WINDOW_H