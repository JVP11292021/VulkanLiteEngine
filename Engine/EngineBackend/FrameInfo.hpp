#ifndef VLE_FRAME_INFO_H
#define VLE_FRAME_INFO_H

#include "defs.hpp"
#include "Camera.hpp"
#include "Object.hpp"

#include <vulkan/vulkan.h>

VLE_NS_B

#define VLE_MAX_LIGHTS 10

struct PointLight {
	glm::vec4 position{};
	glm::vec4 color{};

};

struct GlobalUbo {
	glm::mat4 projection{ 1.f };
	glm::mat4 view{ 1.f };

	glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, 0.2f };
	PointLight pointLights[VLE_MAX_LIGHTS];
	std::int32_t numLights;
};

struct FrameInfo {
	std::int32_t frameIndex;
	float frameTime;
	VkCommandBuffer commandBuffer;
	Camera& camera;
	VkDescriptorSet globalDescriptorSet;
	ObjectMap& gameObjects;
};

VLE_NS_E

#endif // VLE_FRAME_INFO_H