#ifndef VLE_POINT_LIGHT_SYSTEM_H
#define VLE_POINT_LIGHT_SYSTEM_H

#include "engdefs.hpp"
#include "RenderSystem.hpp"

#include <iostream>

#include <Device.hpp>
#include <defs.hpp>
#include <Pipeline.hpp>
#include <Object.hpp>
#include <Camera.hpp>
#include <FrameInfo.hpp>
#include <array>

VLE_SYS_NS_B

struct PointLightPushConstant {
	glm::vec4 position{};
	glm::vec4 color{};
	float radius;
};

class PointLightSystem : public RenderSystem<PointLightPushConstant> {
public:
	using Base = RenderSystem<PointLightPushConstant>;
	PointLightSystem(vle::EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);

	PointLightSystem(const PointLightSystem&) = delete;
	PointLightSystem& operator=(const PointLightSystem&) = delete;

public:
	void update(vle::FrameInfo& frameInfo, vle::GlobalUbo& ubo) override;
	void render(vle::FrameInfo& frameInfo) override;

private:
	void createPipeline(VkRenderPass renderPass) override;
};

VLE_SYS_NS_E

#endif // VLE_POINT_LIGHT_SYSTEM_H