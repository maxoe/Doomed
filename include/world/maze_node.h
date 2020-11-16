#pragma once

#include "renderer/mesh.h"
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

#include "renderer/app_shader.h"

class MazeNode
{
public:
    MazeNode() = default;
    MazeNode(MazeNode const&) = delete;
    ~MazeNode()
    {
        for (auto mesh : meshes)
        {
            delete mesh;
        }
    };

    void draw(AppShader& shader);
    void addObject(std::string const& relModelPath);

private:
    std::vector<Mesh*> meshes;
    std::unordered_map<std::string, GLuint> loadedTextures;
};