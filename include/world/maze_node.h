#pragma once

#include "renderer/mesh.h"

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
        for (auto mesh : meshes)
        {
            delete mesh;
        }
    };

    void draw();
    void addObject(std::string const& relModelPath, glm::mat4& modelMatrix = glm::mat4(1.0f));

    Camera& getCamera();

private:
    Camera camera;
    std::vector<Mesh*> meshes;
    std::unordered_map<std::string, GLuint> loadedTextures;
    AppShader shader;
    glm::vec3 lightWorldPos;
    glm::vec3 lightIntensity;
};