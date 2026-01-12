#ifndef VLE_GLFWWINDOW_HPP
#define VLE_GLFWWINDOW_HPP

#include "defs.hpp"

#if VLE_WIN_WINDOWS

#include <GLFW/glfw3.h>
#include "Window.hpp"

VLE_NS_B

class GLFWWindow final : public EngineWindow<GLFWwindow> {
public:
    GLFWWindow(std::int32_t w, std::int32_t h, std::string title) ;
    ~GLFWWindow() override;

    // Window interface
    bool shouldClose() override;
    void pollEvents();

    VkExtent2D getExtent() const override;
    std::int32_t getWidth() const override;
    std::int32_t getHeight() const override;

    bool wasWindowResized() const override;
    void resetWindowResized() override;

    void createWindowSurface(
        VkInstance instance,
        VkSurfaceKHR* surface
    ) override;

    void* getNativeHandle() const { return static_cast<void*>(_win); }
    inline GLFWwindow* getGLFWwindow() const { return this->_win; }

    GLFWwindow* getWindowPtr() override { return _win; }

private:
    static void framebufferResizeCallback(
        GLFWwindow* window,
        int width,
        int height
    );

    void init() override;
};

VLE_NS_E

#endif // VLE_WIN_WINDOWS

#endif // VLE_GLFWWINDOW_HPP