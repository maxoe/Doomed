#include "renderer/mesh.h"
#include "renderer/app_shader.h"
#include "renderer/texture_loader.h"

#include <iostream>
#include <string>

Mesh::Mesh(
    const std::vector<VertexData>& vertexWithData,
    const std::vector<GLuint>& triangleIndices,
    const std::vector<TextureData>& textureData)
    : numIndices(triangleIndices.size())
    , textures(textureData)
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
}

void Mesh::draw(AppShader& shader) const
{
    GLuint diffuseNr = 0;
    GLuint specularNr = 0;

    // ONLY ONE DIFFUSE AND ONE SPECULAR TEXTURE CURRENTLY
    for (GLuint i = 0; i < textures.size(); i++)
    {
        if (!textures[i].isValid)
        {
            std::cerr << "Skipping texture " << textures[i].path << ", not loaded yet" << std::endl;
            continue;
        }

        glActiveTexture(GL_TEXTURE0 + i);  // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string type = textures[i].type;
        if (type == "textureDiffuse")
            number = std::to_string(diffuseNr++);
        else if (type == "textureSpecular")
            number = std::to_string(specularNr++);
        else
        {
            std::cout << "Type " << type << " of texture " << textures[i].path << " was not found"
                      << std::endl;
        }

        shader.setInt("material." + type.append(number), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // draw mesh
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // reset
    glActiveTexture(GL_TEXTURE0);
}

std::vector<TextureData>& Mesh::getTextureData()
{
    return textures;
}
