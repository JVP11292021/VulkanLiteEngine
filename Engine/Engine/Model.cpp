#include "Model.hpp"

#include "Utils.hpp"
#include "Helpers.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cstring>
#include <unordered_map>

namespace std {
template<>
struct hash<vle::ShaderModel::Vertex> {
	size_t operator()(vle::ShaderModel::Vertex const& vertex) const {
		size_t seed = 0;
		vle::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
		return seed;
	}
};
} // End namespace std

VLE_NS_B

std::vector<VkVertexInputBindingDescription> ShaderModel::Vertex::getBindingDescription() {
	std::uint32_t initial = 1u;
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(initial);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(ShaderModel::Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> ShaderModel::Vertex::getAttributeDescription() {
	return VertexAttributeBuilder{}
		.add<ShaderModel::Vertex>(VK_FORMAT_R32G32B32_SFLOAT, &ShaderModel::Vertex::position)
		.add<ShaderModel::Vertex>(VK_FORMAT_R32G32B32_SFLOAT, &ShaderModel::Vertex::color)
		.add<ShaderModel::Vertex>(VK_FORMAT_R32G32B32_SFLOAT, &ShaderModel::Vertex::normal)
		.add<ShaderModel::Vertex>(VK_FORMAT_R32G32_SFLOAT, &ShaderModel::Vertex::uv)
		.build();
}

void ShaderModel::Builder::loadModel(const std::string& filePath) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
		throw std::runtime_error(warn + err);
	}

	this->vertices.clear();
	this->indices.clear();

	std::unordered_map<Vertex, std::uint32_t> uniqueVertices{};
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};
		
			if (index.texcoord_index >= 0)
				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1]
				};


			if (index.vertex_index >= 0) {
				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2],
				};

				vertex.color = {
					attrib.colors[3 * index.vertex_index + 0],
					attrib.colors[3 * index.vertex_index + 1],
					attrib.colors[3 * index.vertex_index + 2],
				};
			}

			if (index.normal_index >= 0) {
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2],
				};
			}

			if (index.texcoord_index >= 0) {
				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1],
				};
			}

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<std::uint32_t>(vertices.size());
				this->vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}
}

ShaderModel::ShaderModel(EngineDevice& device, const ShaderModel::Builder& builder)
	: _device(device)
{
	this->createVertexBuffers(builder.vertices);
	this->createIndexBuffers(builder.indices);
}

ShaderModel::~ShaderModel() {}


void ShaderModel::bind(VkCommandBuffer commandBuffer) {
	VkBuffer buffers[] = { this->_vertexBuffer->getBuffer()};
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

	if (this->_hasIndexBuffer) {
		vkCmdBindIndexBuffer(commandBuffer, this->_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}
}

void ShaderModel::draw(VkCommandBuffer commandBuffer) {
	if (this->_hasIndexBuffer)
		vkCmdDrawIndexed(commandBuffer, this->_indexCount, 1, 0, 0, 0);
	else
		vkCmdDraw(commandBuffer, this->_vertexCount, 1, 0, 0);
}

std::unique_ptr<ShaderModel> ShaderModel::createModelFromFile(EngineDevice& device, const std::string& filePath) {
	Builder builder{};
	builder.loadModel(filePath);
	return std::make_unique<ShaderModel>(device, builder);
}

void ShaderModel::createVertexBuffers(const std::vector<Vertex>& vertices) {
	this->_vertexCount = static_cast<std::uint32_t>(vertices.size());
	assert(this->_vertexCount >= 3 && "Vertex count must be at least 3 (Triangle)");
	VkDeviceSize bufferSize = sizeof(vertices[0]) * this->_vertexCount;
	std::uint32_t vertexSize = sizeof(vertices[0]);

	Buffer stagingBuffer{
		this->_device,
		vertexSize,
		this->_vertexCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void*)vertices.data());

	this->_vertexBuffer = std::make_unique<Buffer>(
		this->_device,
		vertexSize,
		this->_vertexCount,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


	this->_device.copyBuffer(stagingBuffer.getBuffer(), this->_vertexBuffer->getBuffer(), bufferSize);
}

void ShaderModel::createIndexBuffers(const std::vector<std::uint32_t>& indices) {
	this->_indexCount = static_cast<std::uint32_t>(indices.size());
	this->_hasIndexBuffer = this->_indexCount > 0;

	if (!this->_hasIndexBuffer) return;

	VkDeviceSize bufferSize = sizeof(indices[0]) * this->_indexCount;
	std::uint32_t indexSize = sizeof(indices[0]);

	Buffer stagingBuffer{
		this->_device,
		indexSize,
		this->_indexCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void*)indices.data());

	this->_indexBuffer = std::make_unique<Buffer>(
		this->_device,
		indexSize,
		this->_indexCount,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	this->_device.copyBuffer(stagingBuffer.getBuffer(), this->_indexBuffer->getBuffer(), bufferSize);

}

VLE_NS_E