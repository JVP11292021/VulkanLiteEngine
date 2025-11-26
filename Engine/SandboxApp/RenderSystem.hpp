#ifndef APP_VLE_RENDER_SYSTEM_H
#define APP_VLE_RENDER_SYSTEM_H

#include <iostream>

#include <Device.hpp>
#include <defs.hpp>
#include <Pipeline.hpp>
#include <Object.hpp>

#include <memory>
#include <stdexcept>
#include <array>

class SimpleRenderSystem {
public:

	SimpleRenderSystem(vle::EngineDevice& device, VkRenderPass renderPass);
	~SimpleRenderSystem();

	SimpleRenderSystem(const SimpleRenderSystem&) = delete;
	SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

	void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<vle::Object>& objects);

private:
	void createPipelineLayout();
	void createPipeline(VkRenderPass renderPass);

private:
	vle::EngineDevice& device;
	std::unique_ptr<vle::Pipeline> pipeline;
	VkPipelineLayout pipelineLayout;
};


#endif // APP_VLE_RENDER_SYSTEM_H