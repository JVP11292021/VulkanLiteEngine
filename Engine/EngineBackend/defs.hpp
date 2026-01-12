#ifndef VLE_DEFS_H
#define VLE_DEFS_H

#include <cstdlib>
#include <cinttypes>
#include <exception>
#include <cassert>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#define VLE_NS_B namespace vle {
#define VLE_NS_E }

#ifndef VLE_PLATFORM
#define VLE_PLATFORM            0x00000000
#endif

#ifndef VLE_PLATFORM_WINDOWS
#define VLE_PLATFORM_WINDOWS    0x00010000
#endif

#ifndef VLE_PLATFORM_ANDROID
#define VLE_PLATFORM_ANDROID    0x00020000
#endif

// TODO: Remove this line to enable platform detection
//#ifndef VLE_FORCE_ANDROID
//#define VLE_FORCE_ANDROID
//#endif

#if defined(VLE_FORCE_ANDROID)
	#define VLE_ANDROID
	#define VLE_PLATFORM VLE_PLATFORM_ANDROID
#elif defined(VLE_FORCE_WINDOWS)
	#define VLE_DESKTOP
	#define VLE_PLATFORM VLE_PLATFORM_WINDOWS
#elif defined(_WIN32) || defined(_WIN64)
	#define VLE_DESKTOP
	#define VLE_PLATFORM VLE_PLATFORM_WINDOWS
#elif defined(__ANDROID__)
	#define VLE_ANDROID
	#define VLE_PLATFORM VLE_PLATFORM_ANDROID
#endif

#define VLE_WIN_WINDOWS VLE_PLATFORM == VLE_PLATFORM_WINDOWS
#define	VLE_WIN_ANDROID VLE_PLATFORM == VLE_PLATFORM_ANDROID

#if VLE_WIN_ANDROID
	#include <vulkan/vulkan.h>	
	#include <vulkan/vulkan_android.h>

	#include <android/log.h>
	#include <android/native_window.h>
	#include <android/native_window_jni.h>

	#define VLE_LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "VLE-NFI", __VA_ARGS__))
	#define VLE_LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "VLE-NFI", __VA_ARGS__))
	#define VLE_LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "VLE-NFI", __VA_ARGS__))
#else
	#include <vulkan/vulkan.h>
	#define VLE_LOGI(...)
	#define VLE_LOGW(...)
	#define VLE_LOGE(...)
#endif

#endif // VLE_DEFS_H