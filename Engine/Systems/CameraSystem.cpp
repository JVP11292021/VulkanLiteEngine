#include "CameraSystem.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <corecrt_math_defines.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

VLE_SYS_NS_B

CameraSystem::CameraSystem(const glm::vec3& position,
    const glm::vec3& world_up,
    float pitch,
    float yaw
)
    : position_(position),
    origin_(position),
    world_up_(glm::vec3(0.0f, 1.0f, 0.0f)),
    pitch_(pitch),
    yaw_(yaw),
    front_(glm::vec3(1.0f, 0.0f, 0.0f)),
    movement_speed_(8.5f),
	rotation_speed_(100.0f),
    mouse_sensitivity_(kSensitivity),
    zoom_(kZoom)
{
    this->updateCameraVectors();
}

glm::mat4 CameraSystem::getViewMatrix() const {
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 CameraSystem::getProjMatrix() const {
    float image_width = image_width_;
    float image_height = image_height_;

    glm::mat4 persp;
    persp[0] = glm::vec4(fx_ / image_width, 0.0f, 0.0f, 0.0f);
    persp[1] = glm::vec4(0.0f, fy_ / image_height, 0.0f, 0.0f);
    persp[2] = glm::vec4(-(1 - cx_ / image_width), -cy_ / image_height, near_ + far_, -1.0f);
    persp[3] = glm::vec4(0.0f, 0.0f, near_ * far_, 0.0f);

    glm::mat4 ortho;
    ortho[0] = glm::vec4(2.0f, 0.0f, 0.0f, 0.0f);
    ortho[1] = glm::vec4(0.0f, 2.0f, 0.0f, 0.0f);
    ortho[2] = glm::vec4(0.0f, 0.0f, -2.0f / (far_ - near_), 0.0f);
    ortho[3] = glm::vec4(-1.0f, -1.0f, -(far_ + near_) / (far_ - near_), 1.0f);

    glm::mat4 proj_full = ortho * persp;

    return proj_full;

}

float CameraSystem::getZoom() const {
    return zoom_;
}

void CameraSystem::processKeyboard(CameraSystemMovement move, float dt)
{
    float move_delta = movement_speed_ * dt;
    float rot_delta  = rotation_speed_ * dt;
    bool rotated = false;

    switch (move) {
    case FORWARD:  position_ += front_ * move_delta; break;
    case BACKWARD: position_ -= front_ * move_delta; break;
    case LEFT:     position_ -= right_ * move_delta; break;
    case RIGHT:    position_ += right_ * move_delta; break;
    case MOVE_UP:  position_ += world_up_ * move_delta; break;
    case MOVE_DOWN:position_ -= world_up_ * move_delta; break;

    case ROTATE_UP:    pitch_ -= rot_delta; rotated = true; break;
    case ROTATE_DOWN:  pitch_ += rot_delta; rotated = true; break;
    case ROTATE_LEFT:  yaw_ -= rot_delta; rotated = true; break;
    case ROTATE_RIGHT: yaw_ += rot_delta; rotated = true; break;


    case MOVE_ORIGIN:
        position_ = origin_;
        break;

    case MOVE_TOP:
        position_ = glm::vec3(0.0f, 0.0f, 15.0f) * scale_;
        setDirection(glm::vec3(0.0f));
        break;

    case MOVE_SIDEWAYS_RIGHT:
        position_ = glm::vec3(-5.0f, -5.0f, 5.0f) * scale_;
        setDirection(glm::vec3(0.0f));
        break;

    case MOVE_SIDEWAYS_LEFT:
        position_ = glm::vec3(-5.0f, 5.0f, 5.0f) * scale_;
        setDirection(glm::vec3(0.0f));
        break;
    }

    if (rotated)
        updateCameraVectors(true);
}

void CameraSystem::processMouseInput(float xoffset, float yoffset,
    bool constrain_pitch) {
    pitch_ -= mouse_sensitivity_ * xoffset;
    yaw_ += mouse_sensitivity_ * yoffset;
    this->updateCameraVectors(constrain_pitch);
}

void CameraSystem::processMouseScroll(float yoffset) {
    zoom_ -= yoffset;
    if (zoom_ < 1.0f) {
        zoom_ = 1.0f;
    }
    else if (zoom_ > 45.0f) {
        zoom_ = 45.0f;
    }
    // std::cout << "CameraSystem: zoom = " << zoom_ << std::endl;
}

void CameraSystem::updateCameraVectors(bool constrain_pitch) {
    if (constrain_pitch) {
        if (pitch_ > 89.0f) pitch_ = 89.0f;
        if (pitch_ < -89.0f) pitch_ = -89.0f;
    }

    glm::vec3 front;
    front.x = cos(glm::radians(pitch_)) * cos(glm::radians(yaw_));
    front.y = sin(glm::radians(pitch_));
    front.z = cos(glm::radians(pitch_)) * sin(glm::radians(yaw_));
    front_ = glm::normalize(front);

    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_ = glm::normalize(glm::cross(right_, front_));
}


void CameraSystem::setDirection(const glm::vec3& direction_to) {
    glm::vec3 direction = glm::normalize(direction_to - position_);

    yaw_ = glm::degrees(asin(direction.y));
    pitch_ = glm::degrees(atan2(direction.z, direction.x));
    updateCameraVectors();
}

void CameraSystem::setPosition(const glm::vec3& position) {
    position_ = position;
}

void CameraSystem::setOrigin(const glm::vec3& origin) {
    origin_ = origin;
    this->setPosition(origin_);
}


void CameraSystem::setRotation(const float x_angle, const float y_angle, const float z_angle) {
    yaw_ = glm::degrees(y_angle /* + M_PI_2 */);
    pitch_ = glm::degrees(z_angle + M_PI_2);
    updateCameraVectors();

    // glm::mat4 rotation(1.0f);
    // rotation = glm::rotate(rotation, z_angle, glm::vec3(0.0f, 0.0f, 1.0f));
    // rotation = glm::rotate(rotation, static_cast<float>(M_PI_2), glm::vec3(0.0f, 1.0f, 0.0f));
}

void CameraSystem::setRotationPitchYaw(const float pitch_deg, const float yaw_deg) {
    yaw_ = pitch_deg;
    pitch_ = yaw_deg;
    updateCameraVectors();
}

void CameraSystem::setScale(const float scale) {
    scale_ = scale;
}

void CameraSystem::setIntrinsics(
    const float fx, const float fy, const float cx,
    const float cy, const float wr = 1.0f
) {
    fx_ = fx;
    fy_ = fy;
    cx_ = cx;
    cy_ = cy;
    wr_ = wr;
}

void CameraSystem::setIntrinsics(const CameraSystemIntrinsics& CameraSystem_intr) {
    this->setIntrinsics(CameraSystem_intr.fx, CameraSystem_intr.fy, CameraSystem_intr.cx, CameraSystem_intr.cy, CameraSystem_intr.wr);
}

void CameraSystem::print(std::ostream& os) const {
    std::cout << "CameraSystem: p, y = " << yaw_ << ", " << pitch_ << std::endl;
    std::cout << "CameraSystem: position_ = " << glm::to_string(position_) << std::endl;
}

CameraSystemIntrinsics CameraSystem::getCameraIntrinsics() const {
    CameraSystemIntrinsics intr;
    intr.fx = fx_ / image_width_;
    intr.fy = fy_ / image_height_;
    intr.s = 0.0f;
    intr.cx = cx_ / image_width_;
    intr.cy = cy_ / image_height_;
    intr.wr = wr_;
    return intr;
}

std::ostream& operator<<(std::ostream& os, const CameraSystemIntrinsics& intr) {
    intr.print(os);
    return os;
}

VLE_SYS_NS_E