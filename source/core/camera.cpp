#include "core/camera.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera()
    : camWorldPos(0.0f, 0.0f, 0.0f)
    , cameraWorldDir(glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)))
    , up(glm::vec3(0.0f, 1.0f, 0.0f))
    , yaw(glm::degrees(glm::atan(cameraWorldDir.z, cameraWorldDir.x)))
    , pitch(glm::degrees(glm::asin(cameraWorldDir.y)))
    , fov(45.0f)
    , aspectRatio(3840.f / 2160.f)
    , near(0.1f)
    , far(100.0f)
{
}

glm::vec3 Camera::getCamWorldPos() const
{
    return camWorldPos;
}

glm::vec3 Camera::getCameraWorldDir() const
{
    return cameraWorldDir;
}

glm::mat4 Camera::getView() const
{
    return glm::lookAt(camWorldPos, camWorldPos + cameraWorldDir, up);
}

glm::mat4 Camera::getProjection() const
{
    return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

glm::mat4 Camera::getVP() const
{
    return getProjection() * getView();
}

void Camera::setCamWorldPos(const glm::vec3& pos)
{
    camWorldPos = pos;
}

void Camera::setCameraWorldDir(const glm::vec3& dir)
{
    cameraWorldDir = glm::normalize(dir);
}

void Camera::move(const glm::vec2& deltaPos)
{
    camWorldPos += cameraWorldDir * deltaPos.x;
    camWorldPos += glm::normalize(glm::cross(cameraWorldDir, up)) * deltaPos.y;
}

void Camera::addYaw(float deltaDegrees)
{
    yaw += deltaDegrees;
    cameraWorldDir.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    cameraWorldDir.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
}

void Camera::addPitch(float deltaDegrees)
{
    pitch += deltaDegrees;

    pitch = pitch > 89.0f ? 89.0f : pitch;
    pitch = pitch < -89.0f ? -89.0f : pitch;

    cameraWorldDir.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    cameraWorldDir.y = glm::sin(glm::radians(pitch));
    cameraWorldDir.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
}