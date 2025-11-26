#include "RenderSystem.hpp"

SimpleRenderSystem::SimpleRenderSystem(vle::EngineDevice& device, VkRenderPass renderPass) 
	: device(device)
{
	this->createPipelineLayout();
	this->createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
	vkDestroyPipelineLayout(this->device.device(), pipelineLayout, nullptr);
}

void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<vle::Object>& objects) {
	this->pipeline->bind(commandBuffer);

	for (auto& obj : objects) {
		obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<float>());
		obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.005f, glm::two_pi<float>());

		vle::SimplePushConstantData push{};
		push.color = obj.color;
		push.transform = obj.transform.mat4();
		vkCmdPushConstants(commandBuffer, pipelineLayout, VLE_PUSH_CONST_VERT_FRAG_FLAG, 0, sizeof(vle::SimplePushConstantData), &push);
		obj.model->bind(commandBuffer);
		obj.model->draw(commandBuffer);
	}

}

void SimpleRenderSystem::createPipelineLayout() {

	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VLE_PUSH_CONST_VERT_FRAG_FLAG;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(vle::SimplePushConstantData);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
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