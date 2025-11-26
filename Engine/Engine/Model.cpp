#include "Model.hpp"

#include <cstring>

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
	std::uint32_t initial = 2u;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions(initial);
	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].offset = offsetof(ShaderModel::Vertex, position);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].offset = offsetof(ShaderModel::Vertex, color);
	return attributeDescriptions;
}

ShaderModel::ShaderModel(EngineDevice& device, const std::vector<Vertex>& vertices)
	: _device(device)
{
	this->createVertexBuffers(vertices);
}

ShaderModel::~ShaderModel() {
	vkDestroyBuffer(this->_device.device(), this->_vertexBuffer, nullptr);
	vkFreeMemory(this->_device.device(), this->_vertexBufferMemory, nullptr);
}


void ShaderModel::bind(VkCommandBuffer commandBuffer) {
	VkBuffer buffers[] = { this->_vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void ShaderModel::draw(VkCommandBuffer commandBuffer) {
	vkCmdDraw(commandBuffer, this->_vertexCount, 1, 0, 0);
}


void ShaderModel::createVertexBuffers(const std::vector<Vertex>& vertices) {
	this->_vertexCount = static_cast<std::uint32_t>(vertices.size());
	assert(this->_vertexCount >= 3 && "Vertex count must be at least 3 (Triangle)");
	VkDeviceSize bufferSize = sizeof(vertices[0]) * this->_vertexCount;
	this->_device.createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		this->_vertexBuffer,
		this->_vertexBufferMemory);

	void* data;
	VkDeviceSize offset = static_cast<VkDeviceSize>(0uLL);
	VkMemoryMapFlags flags = static_cast<VkMemoryMapFlags>(0);
	vkMapMemory(this->_device.device(), this->_vertexBufferMemory, offset, bufferSize, flags, &data);
	memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(this->_device.device(), this->_vertexBufferMemory);
}

VLE_NS_E