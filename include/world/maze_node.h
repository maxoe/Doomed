#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

#include "renderer/app_shader.h"
#include "core/app_camera.h"
#include "renderer/model.h"
#include <world/portal.h>

enum class AttachmentPoint
{
    PosX = 0,
    NegX,
    PosY,
    NegY,
    PosZ,
    NegZ
};

class MazeNode
{
public:
    MazeNode();
    MazeNode(const MazeNode&) = delete;

    ~MazeNode()
    {
        for (auto* model : models)
        {
            delete model;
        }

        for (auto* portal : portals)
        {
            delete portal;
        }
    }

    void setLightUniforms(const AppShader& shader) const;
    void draw(AppShader& shader, GLuint nextFreeTextureUnit = 0) const;
    void drawPortals(AppShader& shader, GLuint nextFreeTextureUnit = 0) const;
    void update();

    MazeNode* addPortal(const glm::vec3& pos, const glm::vec3& dir, float width, float height);

    MazeNode* addPortal(
        const glm::vec3& pos,
        const glm::vec3& dir,
        float width,
        float height,
        const glm::vec3& posInTarget,
        const glm::vec3& cameraDirectionInTarget);

    MazeNode* addPortal(
        std::size_t destination,
        const glm::vec3& pos,
        const glm::vec3& dir,
        float width,
        float height,
        bool seemless);

    MazeNode* addPortal(
        const std::string& relModelPath,
        std::size_t destination,
        const glm::vec3& pos,
        const glm::vec3& dir,
        float width,
        float height,
        bool seemless);

    MazeNode* addPortal(
        std::size_t destination,
        const glm::vec3& pos,
        const glm::vec3& dir,
        float width,
        float height,
        const glm::vec3& posInTarget,
        const glm::vec3& cameraDirectionInTarget,
        bool seemless);

    [[nodiscard]] std::vector<Portal*>& getPortals();

    MazeNode* addModel(const std::string& relModelPath);
    MazeNode* addModel(const std::string& relModelPath, const glm::mat4& modelMatrix);
    MazeNode* addModel(const std::string& relModelPath, float maxDimensionLength);
    MazeNode* attachModelToLast(
        const std::string& relModelPath,
        AttachmentPoint ap,
        bool preserveRotation = false);

    MazeNode* addPointLight(
        const glm::vec3& pos,
        const glm::vec3& intensity,
        float dist,
        bool shadows = false);

    MazeNode* MazeNode::addPointLight(
        const glm::vec3& pos,
        const glm::vec3& intensity,
        float dist,
        bool shadows,
        const std::vector<glm::vec4>& initKeyframes);

    MazeNode* addCube(const glm::vec3& pos, float size, const glm::vec3& color);
    MazeNode* addWall(
        std::size_t length,
        std::size_t width,
        const glm::vec3& dir,
        const glm::vec3& sndDir,
        const glm::vec3& leftBottomCorner,
        const glm::vec3& color,
        const glm::vec3& sndColor = glm::vec3(1.0f));
    MazeNode* addOpenBox(
        std::size_t length,
        std::size_t width,
        std::size_t height,
        const glm::vec3& dir,
        const glm::vec3& sndDir,
        const glm::vec3& leftBottomCorner,
        const glm::vec3& color,
        const glm::vec3& sndColor = glm::vec3(1.0f));
    MazeNode* addBox(
        std::size_t length,
        std::size_t width,
        std::size_t height,
        const glm::vec3& dir,
        const glm::vec3& sndDir,
        const glm::vec3& leftBottomCorner,
        const glm::vec3& color,
        const glm::vec3& sndColor = glm::vec3(1.0f));

    std::vector<PointLight>& getPointLights();

    MazeNode* setDirectionalLight(const glm::vec3& dir, const glm::vec3& intensity);
    [[nodiscard]] glm::vec3 MazeNode::getDirectionalLightDirection() const;
    [[nodiscard]] glm::vec3 getDirectionalLightIntensity() const;
    [[nodiscard]] bool getHasDirectionalLight() const;

    void setIsVisible(bool visible);
    [[nodiscard]] bool getIsVisible() const;

private:
    glm::vec3
    calcAttachmentOffset(const Model* oldModel, const Model* newModel, AttachmentPoint ap) const;

    std::vector<Model*> models;
    std::vector<Portal*> portals;
    std::unordered_map<std::string, GLuint> loadedTextures;

    glm::vec3 ambient = glm::vec3(0.0f);
    glm::vec3 directionalLightDir = glm::vec3(0.0f);
    glm::vec3 directionalLightIntensity = glm::vec3(0.0f);
    bool hasDirectionalLight = false;

    std::vector<PointLight> pointLights;

    const glm::vec3 initialPosition = glm::vec3(0.0f, 1.0f, 0.0f);
    const glm::vec3 initialCameraDirection = glm::vec3(1.0f, 0.0f, 0.0f);

    bool isVisible = false;
};