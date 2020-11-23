#include "core/camera.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera()
{
    camWorldPos = glm::vec3(0.0f, -10.0f, -110.0);
    cameraDir = glm::vec3(1.2f, 1.0f, 2.0f);

    glm::vec3 lightWorldPos(5.0f, 5.0f, 2.0f);
    glm::vec3 lightIntensity(50.0f, 40.0f, 40.0f);

    view = glm::lookAt(camWorldPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(0.0f, -7.0f, -25.0f));

    projection = glm::perspective(glm::radians(45.0f), 3840.0f / 2160.0f, 0.1f, 100.0f);
}

glm::vec3 Camera::getCamWorldPos() const
{
    return camWorldPos;
}

glm::vec3 Camera::getCameraDir() const
{
    return cameraDir;
}

glm::mat4 Camera::getView() const
{
    return view;
}

glm::mat4 Camera::getProjection() const
{
    return projection;
}

glm::mat4 Camera::getVP() const
{
    return projection * view;
}

void Camera::setCamWorldPos(const glm::vec3& pos)
{
    camWorldPos = pos;
}

void Camera::setCameraDir(const glm::vec3& dir)
{
    cameraDir = dir;
}
void Camera::setView(const glm::mat4& view)
{
    this->view = view;
}

void Camera::setProjection(const glm::mat4& projection)
{
    this->projection = projection;
}
