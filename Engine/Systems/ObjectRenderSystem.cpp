#include "ObjectRenderSystem.hpp"

VLE_SYS_NS_B

ObjectRenderSystem::ObjectRenderSystem(
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

void ObjectRenderSystem::update(vle::FrameInfo& frameInfo, vle::GlobalUbo& ubo) {}

void ObjectRenderSystem::render(vle::FrameInfo& frameInfo) {
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
		push.color = obj.color;
		vkCmdPushConstants(frameInfo.commandBuffer, this->pipelineLayout, VLE_PUSH_CONST_VERT_FRAG_FLAG, 0, sizeof(SimplePushConstantData), &push);
		obj.model->bind(frameInfo.commandBuffer);
		obj.model->draw(frameInfo.commandBuffer);
	}

}

void ObjectRenderSystem::createPipeline(
        VkRenderPass renderPass,
        const std::string& vertPath,
        const std::string& fragPath
) {
	assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	vle::PipelineConfigInfo pipelineConfig{};
	vle::Pipeline::defaultPipelineConfigInfo(
		pipelineConfig);
	pipelineConfig.bindingDescriptions = vle::ShaderModel::Vertex::getBindingDescription();
	pipelineConfig.attributeDescriptors = vle::ShaderModel::Vertex::getAttributeDescription();
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = this->pipelineLayout;
	this->pipeline = std::make_unique<vle::Pipeline>(
            this->device, vertPath, fragPath, pipelineConfig);
}

VLE_SYS_NS_E