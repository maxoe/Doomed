#include "core/app_camera.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

AppCamera::AppCamera()
    : camWorldPos(0.0f, 1.0f, 0.0f)
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

glm::vec3 AppCamera::getCamWorldPos() const
{
    return camWorldPos;
}

glm::vec3 AppCamera::getCameraWorldDir() const
{
    return cameraWorldDir;
}

glm::mat4 AppCamera::getView() const
{
    return glm::lookAt(camWorldPos, camWorldPos + cameraWorldDir, up);
}

glm::mat4 AppCamera::getProjection() const
{
    return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

glm::mat4 AppCamera::getVP() const
{
    return getProjection() * getView();
}

float AppCamera::getYaw() const
{
    return yaw;
}

float AppCamera::getPitch() const
{
    return pitch;
}

void AppCamera::setCamWorldPos(const glm::vec3& pos)
{
    camWorldPos = pos;
}

void AppCamera::setCameraWorldDir(const glm::vec3& dir)
{
    cameraWorldDir = glm::normalize(dir);
    pitch = glm::asin(-cameraWorldDir.y);
    yaw = atan2(cameraWorldDir.x, cameraWorldDir.z);
}

void AppCamera::move(const glm::vec2& deltaPos)
{
    camWorldPos += cameraWorldDir * deltaPos.x;
    camWorldPos += glm::normalize(glm::cross(cameraWorldDir, up)) * deltaPos.y;
}

void AppCamera::setYaw(float degrees)
{
    yaw = degrees;
    cameraWorldDir.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    cameraWorldDir.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
}

void AppCamera::addYaw(float deltaDegrees)
{
    setYaw(deltaDegrees + yaw);
}

void AppCamera::setPitch(float degrees)
{
    pitch = degrees;

    pitch = pitch > 89.0f ? 89.0f : pitch;
    pitch = pitch < -89.0f ? -89.0f : pitch;

    cameraWorldDir.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    cameraWorldDir.y = glm::sin(glm::radians(pitch));
    cameraWorldDir.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
}

void AppCamera::addPitch(float deltaDegrees)
{
    setPitch(pitch + deltaDegrees);
}