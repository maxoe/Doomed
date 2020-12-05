#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
    Camera();

    glm::vec3 getCamWorldPos() const;
    glm::vec3 getCameraWorldDir() const;
    glm::mat4 getView() const;
    glm::mat4 getProjection() const;
    glm::mat4 getVP() const;

    void setCamWorldPos(const glm::vec3& pos);
    void setCameraWorldDir(const glm::vec3& dir);
    void move(const glm::vec2& deltaPos);
    void addYaw(float deltaDegrees);
    void addPitch(float deltaDegrees);

private:
    glm::vec3 camWorldPos;
    glm::vec3 cameraWorldDir;
    glm::vec3 up;
    float yaw;
    float pitch;

    float fov;
    float aspectRatio;
    float near;
    float far;
};
