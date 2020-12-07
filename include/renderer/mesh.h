#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "renderer/app_shader.h"

struct VertexData
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct TextureData
{
    GLuint id;
    std::string path;
    std::string type;
    bool isValid;
};

class Mesh
{
public:
    Mesh() = delete;
    Mesh(
        const std::vector<VertexData>& vertices,
        const std::vector<GLuint>& indices,
        const std::vector<TextureData>& textures);
    ~Mesh()
    {
        glBindVertexArray(vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteVertexArrays(1, &vao);

        for (const auto& t : textures)
        {
            glDeleteTextures(1, &t.id);
        }
    };

    void draw(AppShader& shader) const;

    std::vector<TextureData>& getTextureData();

    [[nodiscard]] const glm::vec3& getMaxValues() const;
    [[nodiscard]] const glm::vec3& getMinValues() const;

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei numIndices = 0;

    std::vector<TextureData> textures;

    glm::vec3 maxValues;
    glm::vec3 minValues;
};