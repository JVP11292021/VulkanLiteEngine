#ifndef VLE_MODEL_H
#define VLE_MODEL_H

#include "defs.hpp"
#include "Device.hpp"
#include "Buffer.hpp"

#include <vector>
#include <string>
#include <memory>

#ifdef VLE_WIN_ANDROID
#include <android/asset_manager.h>
#endif

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
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

#if VLE_WIN_WINDOWS
        void loadModel(const std::string& filePath);
#elif VLE_WIN_ANDROID
        void loadModel(AAssetManager* assetManager, const std::string& filePath);
#endif
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
	uint32_t getVertexCount() const { return this->_vertexCount; }
	uint32_t getIndexCount() const { return this->_indexCount; }

private:
	void createVertexBuffers(const std::vector<Vertex>& vertices);
	void createIndexBuffers(const std::vector<std::uint32_t>& indices);

private:
	EngineDevice& _device;

	// Vertex buffer variables
	std::unique_ptr<Buffer> _vertexBuffer;
	std::uint32_t _vertexCount;

	// Index buffer variables
	bool _hasIndexBuffer = false;
	std::unique_ptr<Buffer> _indexBuffer;
	std::uint32_t _indexCount;
};

VLE_NS_E

#endif // VLE_MODEL_H