#pragma once
#include <glm/glm.hpp>

class Camera
{
public:
    Camera();

    glm::vec3 getCamWorldPos() const;
    glm::vec3 getCameraDir() const;
    glm::mat4 getView() const;
    glm::mat4 getProjection() const;
    glm::mat4 getVP() const;

    void setCamWorldPos(const glm::vec3& pos);
    void setCameraDir(const glm::vec3& dir);
    void setView(const glm::mat4& view);
    void setProjection(const glm::mat4& projection);

private:
    glm::vec3 camWorldPos;
    glm::vec3 cameraDir;
    glm::mat4 view;
    glm::mat4 projection;
};
