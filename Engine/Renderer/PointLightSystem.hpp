#ifndef APP_VLE_POINT_LIGHT_SYSTEM_H
#define APP_VLE_POINT_LIGHT_SYSTEM_H

#include <iostream>

#include <Device.hpp>
#include <defs.hpp>
#include <Pipeline.hpp>
#include <Object.hpp>
#include <Camera.hpp>
#include <FrameInfo.hpp>

#include <memory>
#include <stdexcept>
#include <array>

class PointLightSystem {
public:

	PointLightSystem(vle::EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
	~PointLightSystem();

	PointLightSystem(const PointLightSystem&) = delete;
	PointLightSystem& operator=(const PointLightSystem&) = delete;

	void update(vle::FrameInfo& frameInfo, vle::GlobalUbo& ubo);
	void render(vle::FrameInfo& frameInfo);

private:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

private:
	vle::EngineDevice& device;
	std::unique_ptr<vle::Pipeline> pipeline;
	VkPipelineLayout pipelineLayout;
};

#endif // APP_VLE_POINT_LIGHT_SYSTEM_H