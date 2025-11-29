#ifndef VLE_ORTHO_CAMERA.H
#define VLE_ORTHO_CAMERA

#include "defs.hpp"

VLE_NS_B

class Camera {
public:
	void setOrthoProjection(float left, float right, float top, float bottom, float near, float far);
	void setPerspectiveProjection(float fovy, float aspect, float near, float far);

	inline const glm::mat4& getProjection() const { return this->_projectionMatrix; }

public:
	glm::mat4 _projectionMatrix{ 1.f };
};

VLE_NS_E

#endif // VLE_ORTHO_CAMERA