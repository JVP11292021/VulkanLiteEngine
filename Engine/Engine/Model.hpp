#ifndef VLE_MODEL_H
#define VLE_MODEL_H

#include "defs.hpp"
#include "Device.hpp"

#include <vector>

VLE_NS_B

#define VLE_PUSH_CONST_VERT_FRAG_FLAG VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
#define VLE_PUSH_CONST_VERT_FLAG VK_SHADER_STAGE_VERTEX_BIT
#define VLE_PUSH_CONST_FRAG_FLAG VK_SHADER_STAGE_FRAGMENT_BIT

struct SimplePushConstantData {
	glm::vec2 offset;
	alignas(16) glm::vec3 color;
};

class ShaderModel {
public:
	struct Vertex {
		glm::vec2 position;
		glm::vec3 color;

		static std::vector<VkVertexInputBindingDescription> getBindingDescription();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescription();

	};
public:
	ShaderModel(EngineDevice& device, const std::vector<Vertex>& vertices);
	~ShaderModel();

	ShaderModel(const ShaderModel&) = delete;
	ShaderModel& operator=(const ShaderModel&) = delete;

public:
	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);


private:
	void createVertexBuffers(const std::vector<Vertex>& vertices);

private:
	EngineDevice& _device;
	VkBuffer _vertexBuffer;
	VkDeviceMemory _vertexBufferMemory;
	std::uint32_t _vertexCount;
};

VLE_NS_E

#endif // VLE_MODEL_H