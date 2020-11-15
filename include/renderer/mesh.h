#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "renderer/app_shader.h"

struct VertexData
{
    glm::vec3 position;
    glm::vec3 normal;
};

class Mesh
{
public:
    std::vector<VertexData> vertices;
    std::vector<unsigned int> indices;

    Mesh() = delete;
    Mesh(Mesh const&) = delete;
    Mesh(std::vector<VertexData> const&& vertices, std::vector<GLuint> const&& indices);
    ~Mesh()
    {
        glBindVertexArray(vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteVertexArrays(1, &vao);
    };

    void draw(AppShader& shader);

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei numIndices = 0;
};