#include "AndroidWindow.hpp"

#if VLE_WIN_ANDROID

VLE_NS_B

AndroidWindow::AndroidWindow(ANativeWindow* win, std::int32_t w, std::int32_t h, std::string title)
	: _nativeWindow(win), EngineWindow(w, h, title)
{
}

AndroidWindow::~AndroidWindow() {
//	ANativeWindow_release(_nativeWindow);
}

void AndroidWindow::init() {
	
	//_win = this;
}

bool AndroidWindow::shouldClose() {
	return false;
}

VkExtent2D AndroidWindow::getExtent() const {
	return {
		static_cast<uint32_t>(_width),
		static_cast<uint32_t>(_height)
	};
}

std::int32_t AndroidWindow::getWidth() const {
	return _width;
}

std::int32_t AndroidWindow::getHeight() const {
	return _height;
}

bool AndroidWindow::wasWindowResized() const {
	return _frameBufferResized;
}

void AndroidWindow::resetWindowResized() {
	_frameBufferResized = false;
}

void AndroidWindow::createWindowSurface(
	VkInstance instance,
	VkSurfaceKHR* surface
) {
	if (!_nativeWindow) {
		throw std::runtime_error("ANativeWindow is null");
	}

	VkAndroidSurfaceCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
	createInfo.window = _nativeWindow;

	if (vkCreateAndroidSurfaceKHR(
		instance,
		&createInfo,
		nullptr,
		surface
	) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Android Vulkan surface");
	}
}

void AndroidWindow::framebufferResizeCallback(
	AndroidWindow* window,
	int width,
	int height
) {
	window->_frameBufferResized = true;
	window->_width = width;
	window->_height = height;
}

VLE_NS_E

#endif // VLE_WIN_ANDROID
