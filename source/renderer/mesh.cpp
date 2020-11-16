#include "renderer/mesh.h"
#include "renderer/app_shader.h"
#include "renderer/texture_loader.h"

#include <iostream>
#include <string>

Mesh::Mesh(
    std::vector<VertexData>&& vertexWithData,
    std::vector<GLuint>&& triangleIndices,
    std::vector<TextureData>&& textureData)
    : numIndices(triangleIndices.size())
{
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertexWithData.size() * sizeof(VertexData),
        &vertexWithData[0],
        GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normal));

    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, texCoords));

    // indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        triangleIndices.size() * sizeof(GLuint),
        &triangleIndices[0],
        GL_STATIC_DRAW);

    glBindVertexArray(0);

    // TODO: PUT THIS IN WOLRD CLASS OR SO
    TextureLoader::load(textureData);
}

void Mesh::draw(AppShader& shader)
{
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;

    // ONLY ONE DIFFUSE AND ONE SPECULAR TEXTURE CURRENTLY
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);  // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if (name == "textureDiffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "textureSpecular")
            number = std::to_string(specularNr++);

        shader.setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    // draw mesh
    glUseProgram(shader.getProgramId());
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // reset
    glActiveTexture(GL_TEXTURE0);
}