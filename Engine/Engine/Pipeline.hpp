#ifndef VLE_PIPELINE_H
#define VLE_PIPELINE_H

#include <string>
#include <vector>

#include "defs.hpp"
#include "Device.hpp"

VLE_NS_B

struct PipelineConfigInfo {
	PipelineConfigInfo(const PipelineConfigInfo&) = delete;
	PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

	VkPipelineInputAssemblyStateCreateInfo assemblyInputInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	std::vector<VkDynamicState> dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo dynamicStateInfo;
	VkPipelineLayout pipelineLayout = nullptr;
	VkRenderPass renderPass = nullptr;
	uint32_t subpass = 0;
};

class Pipeline {
public:
	explicit Pipeline(EngineDevice& device, const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
	~Pipeline();

	Pipeline(const Pipeline&) = delete;
	Pipeline& operator=(const Pipeline&) = delete;

	void bind(VkCommandBuffer commandBuffer);
	static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);


private:
	static std::vector<char> readFile(const std::string& path);

	void createGfxPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo& configInfo);
	void createShaderModule(std::vector<char>& code, VkShaderModule* shaderModule);

private: 
	EngineDevice& _device;
	VkPipeline _gfxPipeline;
	VkShaderModule _vertShaderModule;
	VkShaderModule _fragShaderModule;
};

VLE_NS_E

#endif // VLE_PIPELINE_H