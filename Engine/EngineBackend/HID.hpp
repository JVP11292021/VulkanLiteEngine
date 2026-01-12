#ifndef VLE_HID_H
#define VLE_HID_H

#include "defs.hpp"
#include "Window.hpp"
#include "Object.hpp"

#if VLE_WIN_WINDOWS

#include <GLFW/glfw3.h>

VLE_NS_B

class KeyboardMovementController {
public:
	struct KeyMappings {
        std::int32_t moveLeft = GLFW_KEY_A;
        std::int32_t moveRight = GLFW_KEY_D;
        std::int32_t moveForward = GLFW_KEY_W;
        std::int32_t moveBackward = GLFW_KEY_S;
        std::int32_t moveUp = GLFW_KEY_E;
        std::int32_t moveDown = GLFW_KEY_Q;
        std::int32_t lookLeft = GLFW_KEY_LEFT;
        std::int32_t lookRight = GLFW_KEY_RIGHT;
        std::int32_t lookUp = GLFW_KEY_UP;
        std::int32_t lookDown = GLFW_KEY_DOWN;
    };

public:

    void moveInPlainXZ(GLFWwindow* window, float dt, Object& object);

private:
    KeyMappings _keys{};
    float _moveSpeed{ 3.f };
    float _lookSpeed{ 1.5f };
};

VLE_NS_E

#endif // VLE_WIN_WINDOWS

#endif // VLE_HID_H