#ifndef VLE_RENDER_SYSTEM_PARENT_H
#define VLE_RENDER_SYSTEM_PARENT_H

#include "engdefs.hpp"

#include <Device.hpp>
#include <defs.hpp>
#include <Window.hpp>
#include <SwapChain.hpp>
#include <Pipeline.hpp>
#include <FrameInfo.hpp>

#include <type_traits>

VLE_SYS_NS_B

struct EmptyPush {};

template <typename PushConstant = EmptyPush>
class RenderSystem {
public:
	RenderSystem(vle::EngineDevice& device, VkDescriptorSetLayout globalSetLayout) 
		: device(device) 
	{
		this->createPipelineLayout(globalSetLayout);
	}
	virtual ~RenderSystem() {
		vkDestroyPipelineLayout(this->device.device(), this->pipelineLayout, nullptr);
	};

	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;

public:
	virtual void update(vle::FrameInfo& frameInfo, vle::GlobalUbo& ubo) = 0;
	virtual void render(vle::FrameInfo& frameInfo) = 0;

protected:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
		std::vector<VkDescriptorSetLayout> layouts{ globalSetLayout };

		VkPushConstantRange pushRange{};
		pushRange.stageFlags = VLE_PUSH_CONST_VERT_FRAG_FLAG;
		pushRange.offset = 0;
		pushRange.size = sizeof(PushConstant);

		VkPipelineLayoutCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.setLayoutCount = static_cast<std::uint32_t>(layouts.size());
		info.pSetLayouts = layouts.data();

		if constexpr (std::is_same_v<PushConstant, EmptyPush>) {
			info.pushConstantRangeCount = 0;
			info.pPushConstantRanges = nullptr;
		}
		else {
			info.pushConstantRangeCount = 1;
			info.pPushConstantRanges = &pushRange;
		}

		if (vkCreatePipelineLayout(this->device.device(), &info, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	virtual void createPipeline(
            VkRenderPass renderPass,
            const std::string& vertPath,
            const std::string& fragPath) = 0;
	
protected:
	vle::EngineDevice& device;
	std::unique_ptr<vle::Pipeline> pipeline;
	VkPipelineLayout pipelineLayout;
};

VLE_SYS_NS_E

#endif // VLE_RENDER_SYSTEM_PARENT_H