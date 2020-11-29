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

    void draw();

    MazeNode* addModel(const std::string& relModelPath);
    MazeNode* addModel(const std::string& relModelPath, const glm::mat4& modelMatrix);
    MazeNode* attachModelToLast(
        const std::string& relModelPath,
        AttachmentPoint ap,
        bool preserveRotation = false);

    MazeNode* addPointLight(const glm::vec3& pos, const glm::vec3& intensity, float dist);

    MazeNode* setDirectionalLight(const glm::vec3& dir, const glm::vec3& intensity);
    [[nodiscard]] glm::vec3 MazeNode::getDirectionalLightDirection() const;
    [[nodiscard]] glm::vec3 getDirectionalLightIntensity() const;

    Camera& getCamera();

private:
    glm::vec3 MazeNode::calcAttachmentOffset(
        const Model* oldModel,
        const Model* newModel,
        AttachmentPoint ap) const;

    std::vector<Model*> models;
    Camera camera;
    std::unordered_map<std::string, GLuint> loadedTextures;
    AppShader shader;

    glm::vec3 ambient;
    glm::vec3 directionalLightDir;
    glm::vec3 directionalLightIntensity;
    bool hasDirectionalLight = false;

    std::vector<PointLight> pointLights;
};