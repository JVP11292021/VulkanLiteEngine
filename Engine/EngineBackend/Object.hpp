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

	glm::mat2 mat2();
};

struct Transform3dComponent {
	glm::vec3 translation{};
	glm::vec3 scale{ 1.f, 1.f, 1.f };
	glm::vec3 rotation{};

	// Matrix corresponds to translate * Ry * Rx * Rz * scale transform
	// Rotation convention uses tait-bryan angles with axis order Y(1), X(2), Z(3)
	glm::mat4 mat4();
	glm::mat3 normalMatrix();
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