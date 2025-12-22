#ifndef VLE_POINT_CLOUD_RENDER_SYSTEM_H
#define VLE_POINT_CLOUD_RENDER_SYSTEM_H

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

struct PointCloudPushConstantData {
	glm::mat4 modelMatrix{ 1.f };
	glm::mat4 normalMatrix{ 1.f };
	float pointSize = 1.0f;
};

class PointCloudRenderSystem : public RenderSystem<PointCloudPushConstantData>
{
public:
	using Base = RenderSystem<PointCloudPushConstantData>;

	PointCloudRenderSystem(vle::EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);

	PointCloudRenderSystem(const PointCloudRenderSystem&) = delete;
	PointCloudRenderSystem& operator=(const PointCloudRenderSystem&) = delete;

public:
	void update(vle::FrameInfo& frameInfo, vle::GlobalUbo& ubo) override;
	void render(vle::FrameInfo& frameInfo) override;

private:
	void createPipeline(VkRenderPass renderPass) override;
};

VLE_SYS_NS_E

#endif // !VLE_POINT_CLOUD_RENDER_SYSTEM_H
