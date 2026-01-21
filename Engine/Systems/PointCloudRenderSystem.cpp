#include "PointCloudRenderSystem.hpp"
#include "Descriptors.hpp"

VLE_SYS_NS_B

PointCloudRenderSystem::PointCloudRenderSystem(
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

void PointCloudRenderSystem::update(vle::FrameInfo& frameInfo, vle::GlobalUbo& ubo) {}

void PointCloudRenderSystem::render(vle::FrameInfo& frameInfo) {
	this->pipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(
		frameInfo.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		this->pipelineLayout,
		0, 1,
		&frameInfo.globalDescriptorSet,
		0, nullptr);

	for (auto& kv : frameInfo.pointCloud) {
		vle::Object& obj = kv.second;
		if (!obj.model) continue;
		PointCloudPushConstantData push{};
		push.modelMatrix = obj.transform.mat4();
		push.normalMatrix = obj.transform.normalMatrix();
		push.pointSize = 8.0f;
		vkCmdPushConstants(frameInfo.commandBuffer, this->pipelineLayout, VLE_PUSH_CONST_VERT_FRAG_FLAG, 0, sizeof(PointCloudPushConstantData), &push);
		obj.model->bind(frameInfo.commandBuffer);
		obj.model->draw(frameInfo.commandBuffer);
	}

}

void PointCloudRenderSystem::createPipeline(
        VkRenderPass renderPass,
        const std::string& vertPath,
        const std::string& fragPath
) {
	assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	vle::PipelineConfigInfo pipelineConfig{};
	vle::Pipeline::defaultPipelineConfigInfo(
		pipelineConfig);
	pipelineConfig.assemblyInputInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	pipelineConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_POINT;
	pipelineConfig.bindingDescriptions = vle::ShaderModel::Vertex::getBindingDescription();
	pipelineConfig.attributeDescriptors = vle::ShaderModel::Vertex::getAttributeDescription();
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = this->pipelineLayout;
	this->pipeline = std::make_unique<vle::Pipeline>(
            device, vertPath, fragPath, pipelineConfig);
}

VLE_SYS_NS_E