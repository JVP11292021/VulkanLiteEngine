#ifndef VLE_OBJECT_RENDER_SYSTEM_H
#define VLE_OBJECT_RENDER_SYSTEM_H

#include "engdefs.hpp"
#include "RenderSystem.hpp"

#include <iostream>

#include <Device.hpp>
#include <defs.hpp>
#include <Pipeline.hpp>
#include <Object.hpp>
#include <Camera.hpp>
#include <FrameInfo.hpp>

#include <array>

VLE_SYS_NS_B

struct SimplePushConstantData {
	glm::mat4 modelMatrix{ 1.f };
	glm::mat4 normalMatrix{ 1.f };
};

class ObjectRenderSystem : public RenderSystem<SimplePushConstantData> {
public:
	using Base = RenderSystem<SimplePushConstantData>;

	ObjectRenderSystem(
            vle::EngineDevice& device,
            VkRenderPass renderPass,
            VkDescriptorSetLayout globalSetLayout,
            const std::string& vertPath,
            const std::string& fragPath);

	ObjectRenderSystem(const ObjectRenderSystem&) = delete;
	ObjectRenderSystem& operator=(const ObjectRenderSystem&) = delete;

public:
	void update(vle::FrameInfo& frameInfo, vle::GlobalUbo& ubo) override;
	void render(vle::FrameInfo& frameInfo) override;

private:
	void createPipeline(
            VkRenderPass renderPass,
            const std::string& vertPath,
            const std::string& fragPath) override;
};

VLE_SYS_NS_E

#endif // VLE_OBJECT_RENDER_SYSTEM_H