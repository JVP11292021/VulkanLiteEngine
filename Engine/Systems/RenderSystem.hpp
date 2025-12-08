#ifndef APP_VLE_RENDER_SYSTEM_H
#define APP_VLE_RENDER_SYSTEM_H

#include "engdefs.hpp"

#include <iostream>

#include <Device.hpp>
#include <defs.hpp>
#include <Pipeline.hpp>
#include <Object.hpp>
#include <Camera.hpp>
#include <FrameInfo.hpp>

#include <array>

VLE_SYS_NS_B

class SimpleRenderSystem {
public:

	SimpleRenderSystem(vle::EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
	~SimpleRenderSystem();

	SimpleRenderSystem(const SimpleRenderSystem&) = delete;
	SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

	void renderGameObjects(vle::FrameInfo& frameInfo);

private:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

private:
	vle::EngineDevice& device;
	std::unique_ptr<vle::Pipeline> pipeline;
	VkPipelineLayout pipelineLayout;
};

VLE_SYS_NS_E

#endif // APP_VLE_RENDER_SYSTEM_H