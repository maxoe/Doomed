#pragma once

#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include <renderer/mesh.h>
#include "renderer/app_shader.h"

class Model
{
public:
    Model() = default;
    Model(std::vector<Mesh*>& loadedMeshes, const std::string& mPath, const std::string& tDir);

    ~Model()
    {
        for (auto* mesh : meshes)
        {
            delete mesh;
        }
    }

    void draw(AppShader& shader) const;

    void setModelMatrix(glm::mat4& matrix);

    [[nodiscard]] const std::unordered_map<std::string, GLuint>& getTextureRegistry() const;
    [[nodiscard]] const glm::mat4& getModelMatrix() const;

private:
    const std::string modelPath;
    const std::string textureDir;
    std::vector<Mesh*> meshes;
    std::unordered_map<std::string, GLuint> textureRegistry;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat3 normalMatrix = glm::mat3(1.0f);
};