#ifndef VLE_MODEL_H
#define VLE_MODEL_H

#include "defs.hpp"
#include "Device.hpp"

#include <vector>
#include <string>
#include <memory>

VLE_NS_B

#define VLE_PUSH_CONST_VERT_FRAG_FLAG VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
#define VLE_PUSH_CONST_VERT_FLAG VK_SHADER_STAGE_VERTEX_BIT
#define VLE_PUSH_CONST_FRAG_FLAG VK_SHADER_STAGE_FRAGMENT_BIT

class ShaderModel {
public:
	struct Vertex {
		glm::vec3 position;
		glm::vec3 color;
		glm::vec3 normal{};
		glm::vec2 uv{}; 

		static std::vector<VkVertexInputBindingDescription> getBindingDescription();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescription();

		bool operator==(const Vertex& other) const {
			return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
		}
	};

	struct Builder {
		std::vector<Vertex> vertices{};
		std::vector<std::uint32_t> indices{};

		void loadModel(const std::string& filePath);
	};
public:
	ShaderModel(EngineDevice& device, const ShaderModel::Builder& builder);
	~ShaderModel();

	ShaderModel(const ShaderModel&) = delete;
	ShaderModel& operator=(const ShaderModel&) = delete;

public:
	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);

	static std::unique_ptr<ShaderModel> createModelFromFile(EngineDevice& device, const std::string& filePath);

private:
	void createVertexBuffers(const std::vector<Vertex>& vertices);
	void createIndexBuffers(const std::vector<std::uint32_t>& indices);

private:
	EngineDevice& _device;

	// Vertex buffer variables
	VkBuffer _vertexBuffer;
	VkDeviceMemory _vertexBufferMemory;
	std::uint32_t _vertexCount;

	// Index buffer variables
	bool _hasIndexBuffer = false;
	VkBuffer _indexBuffer;
	VkDeviceMemory _indexBufferMemory;
	std::uint32_t _indexCount;
};

VLE_NS_E

#endif // VLE_MODEL_H