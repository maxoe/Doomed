#include "renderer/model.h"

#include "renderer/app_shader.h"
#include "renderer/texture_loader.h"
#include "renderer/mesh.h"

#include <string>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/transform.hpp>

Model::Model(
    std::vector<Mesh*>& loadedMeshes,
    const std::string& mPath,
    const std::string& tDir,
    float maxDimLength)
    : modelPath(mPath)
    , textureDir(tDir)
    , meshes(loadedMeshes)
    , maxValues(-std::numeric_limits<float>::infinity())
    , minValues(std::numeric_limits<float>::infinity())
{
    for (auto* mesh : meshes)
    {
        // load textures of meshes only if not already loaded
        for (auto& texture : mesh->getTextureData())
        {
            texture.path = (std::filesystem::path(textureDir) / texture.path).string();
            auto tex = textureRegistry.find(texture.path);

            if (tex == textureRegistry.end())
            {
                texture.id = TextureLoader::load(texture);
                textureRegistry.insert({texture.path, texture.id});
            }
            else
            {
                texture.id = tex->second;
            }

            texture.isValid = true;
        }

        minValues.x = minValues.x > mesh->getMinValues().x ? mesh->getMinValues().x : minValues.x;
        minValues.y = minValues.y > mesh->getMinValues().y ? mesh->getMinValues().y : minValues.y;
        minValues.z = minValues.z > mesh->getMinValues().z ? mesh->getMinValues().z : minValues.z;

        maxValues.x = maxValues.x < mesh->getMaxValues().x ? mesh->getMaxValues().x : maxValues.x;
        maxValues.y = maxValues.y < mesh->getMaxValues().y ? mesh->getMaxValues().y : maxValues.y;
        maxValues.z = maxValues.z < mesh->getMaxValues().z ? mesh->getMaxValues().z : maxValues.z;
    }

    ownSize = glm::abs(maxValues - minValues);

    if (maxDimLength != std::numeric_limits<float>::infinity())
    {
        resize(maxDimLength);
    }
}

void Model::draw(AppShader& shader, GLuint nextFreeTextureUnit) const
{
    shader.setMat4f("modelMatrix", modelMatrix);
    shader.setMat3f("normalMatrix", normalMatrix);

    for (auto* mesh : meshes)
    {
        mesh->draw(shader, nextFreeTextureUnit);
    }
}

const std::unordered_map<std::string, GLuint>& Model::getTextureRegistry() const
{
    return textureRegistry;
}

void Model::setModelMatrix(const glm::mat4& matrix)
{
    modelMatrix = matrix;
    normalMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));
}

void Model::addModelTransformation(const glm::mat4& matrix)
{
    modelMatrix = matrix * modelMatrix;
    normalMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));
}

void Model::resize(float maxDimensionLength)
{
    auto& s = getObjectSize();
    float maxLength = glm::max(s.x, glm::max(s.y, s.z));

    addModelTransformation(glm::scale(glm::vec3(maxDimensionLength / maxLength)));
}

const glm::mat4& Model::getModelMatrix() const
{
    return modelMatrix;
}

const glm::vec3& Model::getObjectSize() const
{
    return ownSize;
}

glm::vec3 Model::getWorldSize() const
{
    // mirroring in model matrix may cause negative values
    return glm::abs(glm::mat3(modelMatrix) * ownSize);
}

const glm::vec3& Model::getObjectMinValues() const
{
    return minValues;
}

const glm::vec3& Model::getObjectMaxValues() const
{
    return maxValues;
}

glm::vec3 Model::getTranslation() const
{
    return glm::vec3(modelMatrix[3]);
}
