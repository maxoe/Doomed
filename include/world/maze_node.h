#pragma once

#include "renderer/model.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

#include "renderer/app_shader.h"
#include <core/camera.h>

class MazeNode
{
public:
    MazeNode();
    MazeNode(MazeNode const&) = delete;

    ~MazeNode()
    {
        for (auto* model : models)
        {
            delete model;
        }
    }

    void draw();
    void addModel(std::string const& relModelPath);
    void addModel(std::string const& relModelPath, glm::mat4& modelMatrix);

    Camera& getCamera();

private:
    std::vector<Model*> models;
    Camera camera;
    std::unordered_map<std::string, GLuint> loadedTextures;
    glm::vec3 lightWorldPos;
    glm::vec3 lightIntensity;
    AppShader shader;
};