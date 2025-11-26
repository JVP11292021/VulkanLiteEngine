#ifndef VLE_OBJECT_H
#define VLE_OBJECT_H

#include "defs.hpp"

#include "Model.hpp"
#include <memory>
#include <glm/gtc/matrix_transform.hpp>

VLE_NS_B

using id_t = unsigned int;

struct Transform2dComponent {
	glm::vec2 translation{};
	glm::vec2 scale{ 1.f, 1.f };
	float rotation;

	inline glm::mat2 mat2() { 
		const float s = glm::sin(rotation);
		const float c = glm::cos(rotation);

		glm::mat2 rotMat{ {c, s}, {-s, c} };
		glm::mat2 scaleMat{ {scale.x, 0}, {0, scale.y} };
		
		return rotMat * scaleMat; 
	}
};

struct Transform3dComponent {
	glm::vec3 translation{};
	glm::vec3 scale{ 1.f, 1.f, 1.f };
	glm::vec3 rotation{};

	// Matrix corresponds to translate * Ry * Rx * Rz * scale transform
	// Rotation convention uses tait-bryan angles with axis order Y(1), X(2), Z(3)
	inline glm::mat4 mat4() {
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
};

class Object {
public:
	static Object create() {
		static id_t currentId = 0;
		return Object(currentId++);
	}

	inline id_t getId() const { return this->_id; }

	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;
	Object(Object&&) = default;
	Object& operator=(Object&&) = default;

public:
	std::shared_ptr<ShaderModel> model{};
	glm::vec3 color{};
	Transform3dComponent transform;

private:
	Object(id_t objId) : _id(objId) {}

private:
	id_t _id;
};

VLE_NS_E

#endif // VLE_OBJECT_H