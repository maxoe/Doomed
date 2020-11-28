#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

#include "renderer/app_shader.h"
#include "core/camera.h"
#include "renderer/model.h"

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
    MazeNode* addModel(const std::string& relModelPath, glm::mat4& modelMatrix);
    MazeNode* addPointLight(const PointLight& relModelPath);
    MazeNode* setDirectionalLight(const glm::vec3& dir, const glm::vec3& intensity);

    [[nodiscard]] glm::vec3 MazeNode::getDirectionalLightDirection() const;
    [[nodiscard]] glm::vec3 getDirectionalLightIntensity() const;

    Camera& getCamera();

private:
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