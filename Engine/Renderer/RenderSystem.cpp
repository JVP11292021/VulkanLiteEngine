#include "RenderSystem.hpp"

struct SimplePushConstantData {
	glm::mat4 modelMatrix{ 1.f };
	glm::mat4 normalMatrix{ 1.f };
};

SimpleRenderSystem::SimpleRenderSystem(vle::EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
	: device(device)
{
	this->createPipelineLayout(globalSetLayout);
	this->createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
	vkDestroyPipelineLayout(this->device.device(), this->pipelineLayout, nullptr);
}

void SimpleRenderSystem::renderGameObjects(vle::FrameInfo& frameInfo) {
	this->pipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(
		frameInfo.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		this->pipelineLayout,
		0, 1,
		&frameInfo.globalDescriptorSet,
		0, nullptr );

	for (auto& kv: frameInfo.gameObjects) {
		vle::Object& obj = kv.second;
		if (!obj.model) continue;
		SimplePushConstantData push{};
		push.modelMatrix = obj.transform.mat4();
		push.normalMatrix = obj.transform.normalMatrix();
		vkCmdPushConstants(frameInfo.commandBuffer, this->pipelineLayout, VLE_PUSH_CONST_VERT_FRAG_FLAG, 0, sizeof(SimplePushConstantData), &push);
		obj.model->bind(frameInfo.commandBuffer);
		obj.model->draw(frameInfo.commandBuffer);
	}

}

void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VLE_PUSH_CONST_VERT_FRAG_FLAG;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<std::uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(this->device.device(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create pipeline layout");
	}
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
	assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	vle::PipelineConfigInfo pipelineConfig{};
	vle::Pipeline::defaultPipelineConfigInfo(
		pipelineConfig);
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = this->pipelineLayout;
	this->pipeline = std::make_unique<vle::Pipeline>(device, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
}