#include "renderer/mesh.h"
#include "renderer/app_shader.h"
#include "core/logger.h"

#include <string>
#include <core/app.h>

Mesh::Mesh(
    const std::vector<VertexData>& vertexWithData,
    const std::vector<GLuint>& triangleIndices,
    const std::vector<TextureData>& textureData)
    : numIndices(triangleIndices.size())
    , textures(textureData)
    , maxValues(-std::numeric_limits<float>::infinity())
    , minValues(std::numeric_limits<float>::infinity())
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
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexData),
        reinterpret_cast<void*>(offsetof(VertexData, position)));

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexData),
        reinterpret_cast<void*>(offsetof(VertexData, normal)));

    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(VertexData),
        reinterpret_cast<void*>(offsetof(VertexData, texCoords)));

    // indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        triangleIndices.size() * sizeof(GLuint),
        &triangleIndices[0],
        GL_STATIC_DRAW);

    glBindVertexArray(0);

    for (const auto& v : vertexWithData)
    {
        minValues.x = minValues.x > v.position.x ? v.position.x : minValues.x;
        minValues.y = minValues.y > v.position.y ? v.position.y : minValues.y;
        minValues.z = minValues.z > v.position.z ? v.position.z : minValues.z;

        maxValues.x = maxValues.x < v.position.x ? v.position.x : maxValues.x;
        maxValues.y = maxValues.y < v.position.y ? v.position.y : maxValues.y;
        maxValues.z = maxValues.z < v.position.z ? v.position.z : maxValues.z;
    }
}

/*
 * Next free texture unit a.k.a. the k in GL_TEXTURE0 + k
 */
void Mesh::draw(AppShader& shader, GLuint nextFreeTextureUnit) const
{
    App::getInstance()->getGui().numDrawcalls++;

    GLuint diffuseNr = 0;
    GLuint specularNr = 0;

    // ONLY ONE DIFFUSE AND ONE SPECULAR TEXTURE CURRENTLY
    for (GLuint i = 0; i < textures.size(); i++)
    {
        if (!textures[i].isValid)
        {
            LOG_RENDERER_ERROR("Skipping texture " + textures[i].path + ", not loaded yet");
            continue;
        }

        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string type = textures[i].type;
        if (type == "textureDiffuse")
            number = std::to_string(diffuseNr++);
        else if (type == "textureSpecular")
            number = std::to_string(specularNr++);
        else
        {
            LOG_RENDERER_ERROR(
                "Type " + type + " of texture " + textures[i].path + " was not found");
        }

        shader.setInt(type.append(number), nextFreeTextureUnit + i);
        glActiveTexture(GL_TEXTURE0 + nextFreeTextureUnit + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    shader.setInt("diffuseNr", diffuseNr);
    shader.setInt("specularNr", specularNr);

    // draw mesh
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    // reset
    glActiveTexture(GL_TEXTURE0);
}

std::vector<TextureData>& Mesh::getTextureData()
{
    return textures;
}

const glm::vec3& Mesh::getMaxValues() const
{
    return maxValues;
}

const glm::vec3& Mesh::getMinValues() const
{
    return minValues;
}