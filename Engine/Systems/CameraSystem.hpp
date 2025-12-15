#ifndef SFM_CAMERA_SYSTEM_H
#define SFM_CAMERA_SYSTEM_H

#include "engdefs.hpp"
#include <iostream>
#include <glm/glm.hpp>

VLE_SYS_NS_B

enum CameraSystemMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,

	ROTATE_LEFT,
	ROTATE_RIGHT,
	ROTATE_UP,
	ROTATE_DOWN,

    MOVE_ORIGIN,
    MOVE_TOP,
    MOVE_UP,
	MOVE_DOWN,
    MOVE_SIDEWAYS_RIGHT,
    MOVE_SIDEWAYS_LEFT
};

struct CameraSystemIntrinsics {
    float fx;
    float fy;
    float s;
    float cx;
    float cy;
    float wr;
    glm::mat3 getCameraMatrix() const {
        glm::mat3 camera_matrix(1.0);
        camera_matrix[0] = { fx /* *wr */, 0.0, 0.0 };
        camera_matrix[1] = { 0.0, fy, 0.0 };
        camera_matrix[2] = { cx /* *wr*/, cy, 1.0 };
        return camera_matrix;
    }
    void print(std::ostream& os = std::cout) const {
        os << "(fx = " << fx << ", ";
        os << "fy = " << fy << ", ";
        os << "cx = " << cx << ", ";
        os << "cy = " << cy << ", ";
        os << "wr = " << wr << ")";
    }
};

std::ostream& operator<<(std::ostream& os, const CameraSystemIntrinsics& intr);

// camera constant
const float kYaw = 0.0f;
const float kPitch = 0.0f;  // -5.0f    5.0f
const float kSpeed = 60.2f; // 2.5f 60.5f
const float kSensitivity = 0.1f;
const float kZoom = 45.0f;

class CameraSystem {
public:
    CameraSystem(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
        const glm::vec3& world_up = glm::vec3(0.0f, 0.0f, 1.0f),
        float yaw = kYaw, float pitch = kPitch);

    CameraSystem(const CameraSystem&) = delete;
    CameraSystem& operator=(const CameraSystem&) = delete;

public:
    void processKeyboard(CameraSystemMovement camera_movement, float delta_time);
    void processMouseInput(float xoffset, float yoffset, bool constrain_pitch = true);
    void processMouseScroll(float yoffset);

public:
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjMatrix() const;
    float getZoom() const;
    inline glm::vec3 getPosition() const { return position_; };
    inline float getRotationPitch() const { return yaw_; };
    inline float getRotationYaw() const { return pitch_; };

    void setDirection(const glm::vec3& direction_to);
    void setPosition(const glm::vec3& position);
    void setOrigin(const glm::vec3& origin);
    void setRotation(const float x_angle, const float y_angle, const float z_angle);
    void setRotationPitchYaw(const float pitch, const float yaw);
    void setScale(const float scale);

    void print(std::ostream& os = std::cout) const;

    void setIntrinsics(const float fx, const float fy, const float cx,
        const float cy, const float wr);

    void setIntrinsics(const CameraSystemIntrinsics& camera_intr);

    inline float getImageWidth() { return image_width_; }
    inline float getImageHeight() { return image_height_; }

    inline float getCx() const { return cx_; }
    inline float getCy() const { return cy_; }
    inline float getFx() const { return fx_; }
    inline float getFy() const { return fy_; }

    CameraSystemIntrinsics getCameraIntrinsics() const;

private:
    void updateCameraVectors(bool constrain_pitch = true);

private:

    // Intrinsics
    float fx_ = 1450.317230113;
    float fy_ = 1451.184836113;
    float cx_ = 1244.386581025;
    float cy_ = 1013.145997723;

    float image_width_ = 2452;
    float image_height_ = 2056;

    float wr_ = 2452.0f / 2056.0f;


    float near_ = 0.1;
    float far_ = 5000;

    float scale_ = 1.0f;

    // Extrinsics
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 world_up_;

    float pitch_;
    float yaw_;

    float movement_speed_;
	float rotation_speed_;
    float mouse_sensitivity_;

    glm::vec3 origin_;

    float zoom_;
};

VLE_SYS_NS_E

#endif // SFM_CAMERA_SYSTEM_H