#include "PointLightSystem.hpp"

VLE_SYS_NS_B

PointLightSystem::PointLightSystem(
        vle::EngineDevice& device,
        VkRenderPass renderPass,
        VkDescriptorSetLayout globalSetLayout,
        const std::string& vertPath,
        const std::string& fragPath
)
	: Base(device, globalSetLayout)
{
	this->createPipeline(renderPass, vertPath, fragPath);
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

void PointLightSystem::createPipeline(
        VkRenderPass renderPass,
        const std::string& vertPath,
        const std::string& fragPath
) {
	assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	vle::PipelineConfigInfo pipelineConfig{};
	vle::Pipeline::defaultPipelineConfigInfo(
		pipelineConfig);
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = this->pipelineLayout;
	this->pipeline = std::make_unique<vle::Pipeline>(
            device, vertPath, fragPath, pipelineConfig);
}

VLE_SYS_NS_E