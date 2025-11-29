#include "Camera.hpp"

#include <cassert>
#include <limits>

VLE_NS_B

void Camera::setOrthoProjection(float left, float right, float top, float bottom, float near, float far) {
	this->_projectionMatrix = glm::mat4{ 1.0f };
	this->_projectionMatrix[0][0] = 2.f / (right - left);
	this->_projectionMatrix[1][1] = 2.f / (bottom - top);
	this->_projectionMatrix[2][2] = 1.f / (far - near);
	this->_projectionMatrix[3][0] = -(right + left) / (right - left);
	this->_projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
	this->_projectionMatrix[3][2] = -near / (far - near);
}

void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
	assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
	const float tanHalfFovy = tan(fovy / 2.f);
	this->_projectionMatrix = glm::mat4{ 0.0f };
	this->_projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
	this->_projectionMatrix[1][1] = 1.f / (tanHalfFovy);
	this->_projectionMatrix[2][2] = far / (far - near);
	this->_projectionMatrix[2][3] = 1.f;
	this->_projectionMatrix[3][2] = -(far * near) / (far - near);
}

VLE_NS_E