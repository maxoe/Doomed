#pragma once

#include "renderer/model.h"
#include "core/app_camera.h"

class MazeNode;

class Portal
{
public:
    Portal(
        std::size_t fromNode,
        std::size_t toNode,
        const glm::vec3& pos,
        const glm::vec3& dir,
        float width,
        float height,
        const glm::vec3& posInTarget,
        const glm::vec3& cameraDirectionInTarget,
        bool seemless);

    Portal(
        std::size_t fromNode,
        std::size_t toNode,
        const glm::vec3& pos,
        const glm::vec3& dir,
        float width,
        float height,
        bool seemless);

    Portal(
        const std::string& relModelPath,
        std::size_t fromNode,
        std::size_t toNode,
        const glm::vec3& pos,
        const glm::vec3& dir,
        float width,
        float height,
        bool seemless);

    Portal(
        const std::string& relModelPath,
        std::size_t fromNode,
        std::size_t toNode,
        const glm::vec3& pos,
        const glm::vec3& dir,
        float width,
        float height,
        const glm::vec3& posInTarget,
        const glm::vec3& cameraDirectionInTarget,
        bool seemless);

    ~Portal()
    {
        delete portalObject;

        // if (fbo != 0)
        //{
        //    glDeleteFramebuffers(1, &fbo);
        //}

        // if (texture != 0)
        //{
        //    glDeleteTextures(1, &texture);
        //}
    }

    void draw(AppShader& shader, GLuint nextFreeTextureUnit = 0) const;
    bool collide();
    void teleport();

    [[nodiscard]] const glm::mat4& getModelMatrix() const;
    [[nodiscard]] glm::mat4 getVirtualVPMatrix(const AppCamera& camera) const;
    [[nodiscard]] glm::vec3 getVirtualCameraPosition(const AppCamera& camera) const;
    [[nodiscard]] glm::vec3 getVirtualCameraDirection(const AppCamera& camera) const;
    [[nodiscard]] std::size_t getDestinationNode() const;
    [[nodiscard]] glm::vec3 getTargetPosition() const;
    [[nodiscard]] glm::vec3 getTargetDirection() const;
    [[nodiscard]] float getWidth() const;
    [[nodiscard]] float getHeight() const;
    [[nodiscard]] glm::vec2 getSize() const;

private:
    Model* portalObject;
    /*GLuint fbo;
    GLuint texture;*/

    std::size_t from;
    std::size_t target;
    glm::vec3 targetPos;
    glm::vec3 targetDir;
    bool seemless;

    // perpenticular to portal into the direction you'd have to walk through
    glm::vec3 normal;
    glm::vec3 centerPoint;
    float width;
    float height;
    float lastScalarProjOnNormal = 0.0f;
};
