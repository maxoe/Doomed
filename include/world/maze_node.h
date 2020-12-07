#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

#include "renderer/app_shader.h"
#include "core/camera.h"
#include "renderer/model.h"

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
    }

    void setLightUniforms(const AppShader& shader) const;
    void draw(AppShader& shader);

    MazeNode* addModel(const std::string& relModelPath);
    MazeNode* addModel(const std::string& relModelPath, const glm::mat4& modelMatrix);
    MazeNode* addModel(const std::string& relModelPath, float maxDimensionLength);
    MazeNode* attachModelToLast(
        const std::string& relModelPath,
        AttachmentPoint ap,
        bool preserveRotation = false);

    MazeNode* addPointLight(const glm::vec3& pos, const glm::vec3& intensity, float dist);
    const std::vector<PointLight>& getPointLights() const;

    MazeNode* setDirectionalLight(const glm::vec3& dir, const glm::vec3& intensity);
    [[nodiscard]] glm::vec3 MazeNode::getDirectionalLightDirection() const;
    [[nodiscard]] glm::vec3 getDirectionalLightIntensity() const;
    [[nodiscard]] bool getHasDirectionalLight() const;

private:
    glm::vec3
    calcAttachmentOffset(const Model* oldModel, const Model* newModel, AttachmentPoint ap) const;

    std::vector<Model*> models;
    std::unordered_map<std::string, GLuint> loadedTextures;

    glm::vec3 ambient;
    glm::vec3 directionalLightDir;
    glm::vec3 directionalLightIntensity;
    bool hasDirectionalLight = false;

    std::vector<PointLight> pointLights;
};