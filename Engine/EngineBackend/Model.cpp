#include "Model.hpp"

#include <Hash.hpp>
#include <VkHelpers.hpp>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cstring>
#include <unordered_map>
#include <memory>

#ifdef VLE_WIN_ANDROID
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif // VLE_ANDROID

#if VLE_WIN_ANDROID
std::vector<char> readAssetFile(AAssetManager* assetManager, const std::string& filePath) {
    AAsset* asset = AAssetManager_open(assetManager, filePath.c_str(), AASSET_MODE_STREAMING);
    if (!asset) {
        throw std::runtime_error("Failed to open asset file: " + filePath);
    }

    off_t fileSize = AAsset_getLength(asset);
    std::vector<char> buffer(fileSize);

    int bytesRead = AAsset_read(asset, buffer.data(), fileSize);
    if (bytesRead <= 0) {
        AAsset_close(asset);
        throw std::runtime_error("Failed to read asset file: " + filePath);
    }

    AAsset_close(asset);
    return buffer;
}
#endif

namespace std {
	template<>
	struct hash<vle::ShaderModel::Vertex> {
		size_t operator()(vle::ShaderModel::Vertex const& vertex) const {
			size_t seed = 0;
			vle::utils::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
} // End namespace std

VLE_NS_B
#if VLE_WIN_WINDOWS
class Importer {
public:
	using ModelPair = std::pair < std::vector<ShaderModel::Vertex>, std::vector<std::uint32_t>>;

	Importer() {}
	virtual ~Importer() {}

public:
	virtual ModelPair loadObject(const std::string& filePath) = 0;
};
#elifdef VLE_WIN_ANDROID
    class Importer {
    public:
        using ModelPair = std::pair<std::vector<ShaderModel::Vertex>, std::vector<std::uint32_t>>;
        virtual ~Importer() {}

        virtual ModelPair loadObject(const std::string& filePath) = 0;

        // Android-specific: overload that accepts an AAssetManager
        virtual ModelPair loadObject(AAssetManager* assetManager, const std::string& filePath) {
            // Default implementation calls the regular one for desktop-like behavior
            return loadObject(filePath);
        }
    };
#endif

class OBJImporter : public Importer {
public:
	ModelPair loadObject(const std::string& filePath) override {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		std::vector<ShaderModel::Vertex> vertices;
		std::vector<std::uint32_t> indices;
		std::unordered_map<ShaderModel::Vertex, std::uint32_t> uniqueVertices;

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				ShaderModel::Vertex vertex{};

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
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}

		return { vertices, indices };
	}
};

class PLYImporter : public Importer {
public:
#ifdef VLE_WIN_ANDROID
        ModelPair loadObject(AAssetManager* assetManager, const std::string& filePath) {
            auto data = readAssetFile(assetManager, filePath);

            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFileFromMemory(
                    data.data(),
                    data.size(),
                    aiProcess_Triangulate |
                    aiProcess_GenSmoothNormals |
                    aiProcess_FlipUVs |
                    aiProcess_JoinIdenticalVertices,
                    "ply" // important: tells Assimp the format
            );

            if (!scene || !scene->HasMeshes()) {
                throw std::runtime_error(importer.GetErrorString());
            }

            return processScene(scene); // reuse your existing processScene function
        }
#endif

#ifdef VLE_WIN_WINDOWS
        // Desktop / fallback
        ModelPair loadObject(const std::string& filePath) override {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(
                    filePath,
                    aiProcess_Triangulate |
                    aiProcess_GenSmoothNormals |
                    aiProcess_FlipUVs |
                    aiProcess_JoinIdenticalVertices
            );

            if (!scene || !scene->HasMeshes()) {
                throw std::runtime_error("Failed to load model using Assimp: " + filePath);
            }

            return processScene(scene);
        }
#endif // VLE_WIN_WINDOWS

    private:
        ModelPair processScene(const aiScene* scene) {
            std::vector<ShaderModel::Vertex> vertices;
            std::vector<uint32_t> indices;

            for (uint32_t m = 0; m < scene->mNumMeshes; ++m) {
                aiMesh* mesh = scene->mMeshes[m];
                uint32_t base = static_cast<uint32_t>(vertices.size());

                for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
                    ShaderModel::Vertex v{};
                    v.position = { mesh->mVertices[i].x,
                                   mesh->mVertices[i].y,
                                   mesh->mVertices[i].z };

                    if (mesh->HasNormals())
                        v.normal = { mesh->mNormals[i].x,
                                     mesh->mNormals[i].y,
                                     mesh->mNormals[i].z };

                    if (mesh->HasVertexColors(0))
                        v.color = { mesh->mColors[0][i].r,
                                    mesh->mColors[0][i].g,
                                    mesh->mColors[0][i].b };

                    vertices.push_back(v);
                }

                for (uint32_t f = 0; f < mesh->mNumFaces; ++f)
                    for (uint32_t j = 0; j < mesh->mFaces[f].mNumIndices; ++j)
                        indices.push_back(base + mesh->mFaces[f].mIndices[j]);
            }

            return { vertices, indices };
        }
    };

class GLTFImporter : public Importer {
public:
	ModelPair loadObject(const std::string& filePath) override {
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(
			filePath,
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_JoinIdenticalVertices |
			aiProcess_FlipUVs
		);

		if (!scene || !scene->HasMeshes()) {
			throw std::runtime_error("Failed to load glTF: " + filePath);
		}

		std::vector<ShaderModel::Vertex> vertices;
		std::vector<uint32_t> indices;
		std::unordered_map<ShaderModel::Vertex, uint32_t> uniqueVertices;

		for (uint32_t m = 0; m < scene->mNumMeshes; ++m) {
			aiMesh* mesh = scene->mMeshes[m];

			for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
				ShaderModel::Vertex v{};

				v.position = {
					mesh->mVertices[i].x,
					mesh->mVertices[i].y,
					mesh->mVertices[i].z
				};

				if (mesh->HasNormals()) {
					v.normal = {
						mesh->mNormals[i].x,
						mesh->mNormals[i].y,
						mesh->mNormals[i].z
					};
				}

				if (mesh->HasTextureCoords(0)) {
					v.uv = {
						mesh->mTextureCoords[0][i].x,
						mesh->mTextureCoords[0][i].y
					};
				}

				if (mesh->HasVertexColors(0)) {
					v.color = {
						mesh->mColors[0][i].r,
						mesh->mColors[0][i].g,
						mesh->mColors[0][i].b
					};
				}

				if (uniqueVertices.count(v) == 0) {
					uniqueVertices[v] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(v);
				}

				indices.push_back(uniqueVertices[v]);
			}
		}

		return { vertices, indices };
	}
};

std::shared_ptr<Importer> make_importer(const std::string& filePath) {
	auto extPos = filePath.find_last_of('.');
	if (extPos == std::string::npos) throw std::runtime_error("Model file has no extension!");
	std::string ext = filePath.substr(extPos + 1);

	if (ext == "obj")
		return std::make_shared<OBJImporter>();
	else if (ext == "ply")
		return std::make_shared<PLYImporter>();
	else if (ext == "gltf" || ext == "glb")
		return std::make_shared<GLTFImporter>();

	throw std::runtime_error("Unsupported model format: " + ext);
}

std::vector<VkVertexInputBindingDescription> ShaderModel::Vertex::getBindingDescription() {
	std::uint32_t initial = 1u;
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(initial);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(ShaderModel::Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> ShaderModel::Vertex::getAttributeDescription() {
	return vle::utils::VertexAttributeBuilder{}
		.add<ShaderModel::Vertex>(VK_FORMAT_R32G32B32_SFLOAT, &ShaderModel::Vertex::position)
		.add<ShaderModel::Vertex>(VK_FORMAT_R32G32B32_SFLOAT, &ShaderModel::Vertex::color)
		.add<ShaderModel::Vertex>(VK_FORMAT_R32G32B32_SFLOAT, &ShaderModel::Vertex::normal)
		.add<ShaderModel::Vertex>(VK_FORMAT_R32G32_SFLOAT, &ShaderModel::Vertex::uv)
		.build();
}

#if VLE_WIN_WINDOWS
void ShaderModel::Builder::loadModel(const std::string& filePath) {
	std::shared_ptr<Importer> importer = make_importer(filePath);
	auto [vertices, indices] = importer->loadObject(filePath);

	this->vertices = vertices;
	this->indices = indices;
}
#elif VLE_WIN_ANDROID
    void ShaderModel::Builder::loadModel(AAssetManager* assetManager, const std::string& filePath) {
        auto importer = make_importer(filePath);

        // call the AAssetManager-aware overload
        auto [vertices, indices] = importer->loadObject(assetManager, filePath);

        this->vertices = vertices;
        this->indices = indices;
    }
#endif

ShaderModel::ShaderModel(EngineDevice& device, const ShaderModel::Builder& builder)
	: _device(device)
{
	this->createVertexBuffers(builder.vertices);
	this->createIndexBuffers(builder.indices);
}

ShaderModel::~ShaderModel() {}


void ShaderModel::bind(VkCommandBuffer commandBuffer) {
	VkBuffer buffers[] = { this->_vertexBuffer->getBuffer() };
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
#if VLE_WIN_WINDOWS
        builder.loadModel(filePath);
#elif VLE_WIN_ANDROID
        builder.loadModel(device.assetManager(), filePath); // pass the AAssetManager
#endif
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
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };

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