#include "PointLightSystem.hpp"

PointLightSystem::PointLightSystem(vle::EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
	: device(device)
{
	this->createPipelineLayout(globalSetLayout);
	this->createPipeline(renderPass);
}

PointLightSystem::~PointLightSystem() {
	vkDestroyPipelineLayout(this->device.device(), this->pipelineLayout, nullptr);
}

void PointLightSystem::render(vle::FrameInfo& frameInfo) {
	this->pipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(
		frameInfo.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		this->pipelineLayout,
		0, 1,
		&frameInfo.globalDescriptorSet,
		0, nullptr);

	vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);

}

void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

	//VkPushConstantRange pushConstantRange{};
	//pushConstantRange.stageFlags = VLE_PUSH_CONST_VERT_FRAG_FLAG;
	//pushConstantRange.offset = 0;
	//pushConstantRange.size = sizeof(SimplePushConstantData);

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<std::uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(this->device.device(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout");
	}
}

void PointLightSystem::createPipeline(VkRenderPass renderPass) {
	assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	vle::PipelineConfigInfo pipelineConfig{};
	vle::Pipeline::defaultPipelineConfigInfo(
		pipelineConfig);
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = this->pipelineLayout;
	this->pipeline = std::make_unique<vle::Pipeline>(device, "shaders/point_light.vert.spv", "shaders/point_light.frag.spv", pipelineConfig);
}