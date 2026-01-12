#include "GLFWWindow.hpp"
#include <stdexcept>

#if VLE_WIN_WINDOWS

VLE_NS_B

GLFWWindow::GLFWWindow(int w, int h, std::string title)
    : EngineWindow(w, h, title)
{
	this->init();
    if (!_win) {
        throw std::runtime_error("Failed to create GLFW window");
    }
}

GLFWWindow::~GLFWWindow() {
    glfwDestroyWindow(_win);
    glfwTerminate();
}

void GLFWWindow::init() {
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    _win = glfwCreateWindow(
        _width, _height, _title.c_str(), nullptr, nullptr
    );

    if (!_win)
        throw std::runtime_error("Failed to create GLFW window");

    glfwSetWindowUserPointer(_win, this);
    glfwSetFramebufferSizeCallback(_win, framebufferResizeCallback);
}

bool GLFWWindow::shouldClose() {
    return glfwWindowShouldClose(_win);
}

void GLFWWindow::pollEvents() {
    glfwPollEvents();
}

VkExtent2D GLFWWindow::getExtent() const {
    return {
        static_cast<uint32_t>(_width),
        static_cast<uint32_t>(_height)
    };
}

int GLFWWindow::getWidth() const { return _width; }
int GLFWWindow::getHeight() const { return _height; }

bool GLFWWindow::wasWindowResized() const {
    return _frameBufferResized;
}

void GLFWWindow::resetWindowResized() {
    _frameBufferResized = false;
}

void GLFWWindow::createWindowSurface(
    VkInstance instance,
    VkSurfaceKHR* surface
) {
    if (glfwCreateWindowSurface(
        instance, _win, nullptr, surface
    ) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }
}

void GLFWWindow::framebufferResizeCallback(
    GLFWwindow* window, int width, int height
) {
    auto self = reinterpret_cast<GLFWWindow*>(
        glfwGetWindowUserPointer(window)
        );
    self->_frameBufferResized = true;
    self->_width = width;
    self->_height = height;
}

VLE_NS_E

#endif // VLE_WIN_WINDOWS