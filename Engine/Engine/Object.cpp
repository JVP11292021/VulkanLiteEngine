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
			invScale.x* (c1 * c3 + s1 * s2 * s3),
			invScale.x* (c2 * s3),
			invScale.x * (c1 * s2 * s3 - c3 * s1)
		},
		{
			invScale.y* (c3 * s1 * s2 - c1 * s3),
			invScale.y* (c2 * c3),
			invScale.y * (c1 * c3 * s2 + s1 * s3)
		},
		{
			invScale.z* (c2 * s1),
			invScale.z* (-s2),
			invScale.z * (c1 * c2)
		}};
}

VLE_NS_E