#ifndef VLE_OBJECT_H
#define VLE_OBJECT_H

#include "defs.hpp"

#include "Model.hpp"
#include <memory>

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
	Transform2dComponent transform2d;

private:
	Object(id_t objId) : _id(objId) {}

private:
	id_t _id;
};

VLE_NS_E

#endif // VLE_OBJECT_H