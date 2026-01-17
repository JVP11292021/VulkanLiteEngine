#include "Object.hpp"

VLE_NS_B

glm::mat2 Transform2dComponent::mat2() {
	const float s = glm::sin(rotation);
	const float c = glm::cos(rotation);

	glm::mat2 rotMat{ {c, s}, {-s, c} };
	glm::mat2 scaleMat{ {scale.x, 0}, {0, scale.y} };

	return rotMat * scaleMat;
}

glm::mat4 Transform3dComponent::mat4() {
	const float c3 = glm::cos(this->rotation.z);
	const float s3 = glm::sin(this->rotation.z);
	const float c2 = glm::cos(this->rotation.x);
	const float s2 = glm::sin(this->rotation.x);
	const float c1 = glm::cos(this->rotation.y);
	const float s1 = glm::sin(this->rotation.y);
	return glm::mat4{
		{
			this->scale.x * (c1 * c3 + s1 * s2 * s3),
			this->scale.x * (c2 * s3),
			this->scale.x * (c1 * s2 * s3 - c3 * s1),
			0.0f,
		},
		{
			this->scale.y * (c3 * s1 * s2 - c1 * s3),
			this->scale.y * (c2 * c3),
			this->scale.y * (c1 * c3 * s2 + s1 * s3),
			0.0f,
		},
		{
			this->scale.z * (c2 * s1),
			this->scale.z * (-s2),
			this->scale.z * (c1 * c2),
			0.0f,
		},
		{this->translation.x, this->translation.y, this->translation.z, 1.0f} };
}

glm::mat3 Transform3dComponent::normalMatrix() {
	const float c3 = glm::cos(this->rotation.z);
	const float s3 = glm::sin(this->rotation.z);
	const float c2 = glm::cos(this->rotation.x);
	const float s2 = glm::sin(this->rotation.x);
	const float c1 = glm::cos(this->rotation.y);
	const float s1 = glm::sin(this->rotation.y);
	const glm::vec3 invScale = 1.0f / this->scale;

	return glm::mat3{
		{
			invScale.x * (c1 * c3 + s1 * s2 * s3),
			invScale.x * (c2 * s3),
			invScale.x * (c1 * s2 * s3 - c3 * s1)
		},
		{
			invScale.y * (c3 * s1 * s2 - c1 * s3),
			invScale.y * (c2 * c3),
			invScale.y * (c1 * c3 * s2 + s1 * s3)
		},
		{
			invScale.z * (c2 * s1),
			invScale.z * (-s2),
			invScale.z * (c1 * c2)
		}};
}

Object Object::createPointLight(float intensity, float radius, glm::vec3 color) {
	Object obj = Object::create();
	obj.color = color;
	obj.transform.scale.x = radius;
	obj.pointLight = std::make_unique<PointLightComponent3D>();
	obj.pointLight->lightIntensity = intensity;
	return obj;
}

VLE_NS_E

VLE_ENT_NS_B

std::unique_ptr<vle::ShaderModel> MakeModel(vle::EngineDevice& device, const std::vector<ShaderModel::Vertex>& v) {
    vle::ShaderModel::Builder b{};
    b.vertices = v;
    return std::make_unique<vle::ShaderModel>(device, b);
}

std::unique_ptr<vle::ShaderModel> Cube(vle::EngineDevice& device, glm::vec3 offset) {
    std::vector<vle::ShaderModel::Vertex> vertices{
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

    };

    for (auto& v : vertices) {
        v.position += offset;
    }

    return MakeModel(device, vertices);
}

VLE_ENT_NS_E