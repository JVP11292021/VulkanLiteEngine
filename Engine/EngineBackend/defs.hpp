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

#ifndef VLE_PLATFORM_WINDOWS
#define VLE_PLATFORM_WINDOWS    0x00010000
#endif

#ifndef VLE_PLATFORM_ANDROID
#define VLE_PLATFORM_ANDROID    0x00020000
#endif

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
#else
    #error "Could not find the specified platform"
#endif

#define VLE_WIN_WINDOWS VLE_PLATFORM == VLE_PLATFORM_WINDOWS
#define	VLE_WIN_ANDROID VLE_PLATFORM == VLE_PLATFORM_ANDROID

#if VLE_WIN_ANDROID
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_android.h>

#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#define VLE_LOG_TAG "VLE-NFI"

template <size_t N>
constexpr auto make_format()
{
    constexpr char fmt[] = "%s ";
    std::array<char, N * 3> buf{};   // "%s " is 3 chars
    for (size_t i = 0; i < N; ++i)
    {
        buf[i*3+0] = '%';
        buf[i*3+1] = 's';
        buf[i*3+2] = ' ';
    }
    buf[N*3-1] = 0;   // remove trailing space, null-terminate
    return buf;
}

template <typename... Args>
inline void vle_log(android_LogPriority logPriority, Args&&... args)
{
    constexpr auto fmt = make_format<sizeof...(Args)>();
    __android_log_print(logPriority, VLE_LOG_TAG, fmt.data(), args...);
}

#define VLE_LOGI(...) vle_log(ANDROID_LOG_INFO, __VA_ARGS__)
#define VLE_LOGW(...) vle_log(ANDROID_LOG_WARN, __VA_ARGS__)
#define VLE_LOGE(...) vle_log(ANDROID_LOG_ERROR, __VA_ARGS__)
#define VLE_LOGD(...) vle_log(ANDROID_LOG_DEBUG, __VA_ARGS__)
#define VLE_LOGF(...) vle_log(ANDROID_LOG_FATAL, __VA_ARGS__)

#else
	#include <vulkan/vulkan.h>
	#define VLE_LOGI(...)
	#define VLE_LOGW(...)
	#define VLE_LOGE(...)
#endif

#endif // VLE_DEFS_H