#ifndef VLE_ANDROID_WINDOW_HPP
#define VLE_ANDROID_WINDOW_HPP

#include "defs.hpp"

#if VLE_WIN_ANDROID

#include "Window.hpp"

VLE_NS_B

class AndroidWindow final : public EngineWindow<ANativeWindow>{
public:
	AndroidWindow(ANativeWindow* win, std::int32_t w, std::int32_t h, std::string title);
	~AndroidWindow() override;

	// Window interface
	bool shouldClose() override;

	VkExtent2D getExtent() const override;
	std::int32_t getWidth() const override;
	std::int32_t getHeight() const override;

	bool wasWindowResized() const override;
	void resetWindowResized() override;

	void createWindowSurface(
		VkInstance instance,
		VkSurfaceKHR* surface
	) override;

    ANativeWindow* getWindowPtr() override {
        return _nativeWindow;
    }

	void setNativeWindow(ANativeWindow* window) {
		_nativeWindow = window;
		_width = ANativeWindow_getWidth(window);
		_height = ANativeWindow_getHeight(window);
//        _frameBufferResized = true;
	}


private:
	void init() override;
	static void framebufferResizeCallback(
		AndroidWindow* window,
		int width,
		int height
	);
	ANativeWindow* _nativeWindow = nullptr;
};

VLE_NS_E

#endif // VLE_WIN_ANDROID

#endif // VLE_ANDROID_WINDOW_HPP