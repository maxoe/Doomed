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
    std::string type;
    std::string path;
    bool isValid;
};

class Mesh
{
public:
    Mesh() = delete;
    Mesh(Mesh const&) = delete;
    Mesh(
        std::vector<VertexData>&& vertices,
        std::vector<GLuint>&& indices,
        std::vector<TextureData>&& textureData);
    ~Mesh()
    {
        glBindVertexArray(vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteVertexArrays(1, &vao);
    };

    void draw(AppShader& shader);
    std::vector<TextureData>& getTextureData();
    void setModelMatrix(glm::mat4& modelMatrix);
    const glm::mat4& getModelMatrix() const;

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei numIndices = 0;

    // std::vector<VertexData> vertices;
    // std::vector<GLuint> indices;
    std::vector<TextureData> textures;
    glm::mat4 modelMatrix;
    glm::mat4 normalMatrix;
};