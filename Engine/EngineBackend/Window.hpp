#ifndef VLE_WINDOW_H
#define VLE_WINDOW_H

#include <cinttypes>
#include <string>
#include <stdexcept>

#include "defs.hpp"

VLE_NS_B

template <typename Window>
class EngineWindow {
public:
	EngineWindow(std::int32_t w, std::int32_t h, std::string t) 
		: _width(w), 
		_height(h), 
		_frameBufferResized(false),
		_title(std::move(t)),
		_win(nullptr)
	{}

	 virtual ~EngineWindow() {}

	EngineWindow(const EngineWindow&) = delete;
	EngineWindow& operator=(const EngineWindow&) = delete;

public:
	virtual inline bool shouldClose() = 0;
	virtual inline VkExtent2D getExtent() const = 0;
	virtual inline bool wasWindowResized() const = 0;
	virtual inline void resetWindowResized() = 0;
	virtual inline std::int32_t getWidth() const = 0;
	virtual inline std::int32_t getHeight() const = 0;
	virtual inline Window* getWindowPtr() = 0;

public:
	virtual void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) = 0;

protected:
	virtual void init() = 0;

protected:
	Window* _win;

	std::int32_t _width;
	std::int32_t _height;
	bool _frameBufferResized;
	std::string _title;
};

VLE_NS_E

#endif // VLE_WINDOW_H