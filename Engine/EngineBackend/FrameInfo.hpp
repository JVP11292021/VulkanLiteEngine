#ifndef VLE_FRAME_INFO_H
#define VLE_FRAME_INFO_H

#include "defs.hpp"
#include "Camera.hpp"

#include <vulkan/vulkan.h>

VLE_NS_B

struct FrameInfo {
	std::int32_t frameIndex;
	float frameTime;
	VkCommandBuffer commandBuffer;
	Camera& camera;
	VkDescriptorSet globalDescriptorSet;
};

VLE_NS_E

#endif // VLE_FRAME_INFO_H