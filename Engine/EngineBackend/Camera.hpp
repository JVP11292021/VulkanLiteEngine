#ifndef VLE_ORTHO_CAMERA.H
#define VLE_ORTHO_CAMERA

#include "defs.hpp"

VLE_NS_B

class Camera {
public:
	Camera() = default;
	~Camera() = default;

	Camera(const Camera&) = default;
	Camera& operator=(const Camera&) = default;

public:
	void setOrthoProjection(float left, float right, float top, float bottom, float near, float far);
	void setPerspectiveProjection(float fovy, float aspect, float near, float far);

	void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });
	void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });
	void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

	inline const glm::mat4& getProjection() const { return this->_projectionMatrix; }
	inline const glm::mat4& getView() const { return this->_viewMatrix; }

public:
	glm::mat4 _projectionMatrix{ 1.f };
	glm::mat4 _viewMatrix{ 1.f };
};

VLE_NS_E

#endif // VLE_ORTHO_CAMERA