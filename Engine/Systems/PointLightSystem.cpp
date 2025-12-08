#include "PointLightSystem.hpp"

VLE_SYS_NS_B

struct PointLightPushConstant {
	glm::vec4 position{};
	glm::vec4 color{};
	float radius;
};

PointLightSystem::PointLightSystem(vle::EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
	: device(device)
{
	this->createPipelineLayout(globalSetLayout);
	this->createPipeline(renderPass);
}

PointLightSystem::~PointLightSystem() {
	vkDestroyPipelineLayout(this->device.device(), this->pipelineLayout, nullptr);
}

void PointLightSystem::update(vle::FrameInfo& frameInfo, vle::GlobalUbo& ubo) {
	auto rotHeight = glm::rotate(
		glm::mat4(1.f),
		frameInfo.frameTime,
		{ 0.f, 1.f, 0.f });

	std::int32_t lightIndex = 0;
	for (auto& kv : frameInfo.gameObjects) {
		auto& obj = kv.second;
		if (!obj.pointLight) continue;

		obj.transform.translation = glm::vec3(rotHeight * glm::vec4(obj.transform.translation, 1.f));

		ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.f);
		ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
		lightIndex++;
	}

	ubo.numLights = lightIndex;
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

	for (auto& kv : frameInfo.gameObjects) {
		auto& obj = kv.second;
		if (!obj.pointLight) continue;
		
		PointLightPushConstant push{};
		push.position = glm::vec4(obj.transform.translation, 1.f);
		push.color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
		push.radius = obj.transform.scale.x;

		vkCmdPushConstants(
			frameInfo.commandBuffer,
			this->pipelineLayout,
			VLE_PUSH_CONST_VERT_FRAG_FLAG,
			0,
			sizeof(PointLightPushConstant),
			&push);
		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}

}

void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VLE_PUSH_CONST_VERT_FRAG_FLAG;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(PointLightPushConstant);

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

void PointLightSystem::createPipeline(VkRenderPass renderPass) {
	assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	vle::PipelineConfigInfo pipelineConfig{};
	vle::Pipeline::defaultPipelineConfigInfo(
		pipelineConfig);
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = this->pipelineLayout;
	this->pipeline = std::make_unique<vle::Pipeline>(device, "shaders/point_light.vert.spv", "shaders/point_light.frag.spv", pipelineConfig);
}

VLE_SYS_NS_E