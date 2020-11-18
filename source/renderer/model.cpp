#include "renderer/model.h"

#include "renderer/app_shader.h"
#include "renderer/texture_loader.h"
#include "renderer/mesh.h"

#include <string>
#include <glm/gtc/matrix_inverse.hpp>

Model::Model(std::vector<Mesh*>& loadedMeshes, const std::string& mPath, const std::string& tDir)
    : modelPath(mPath)
    , textureDir(tDir)
    , meshes(loadedMeshes)

{
    // load textures of meshes only if not already loaded
    for (auto* mesh : meshes)
    {
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
    }
}

void Model::draw(AppShader& shader) const
{
    shader.setMat4f("modelMatrix", modelMatrix);
    shader.setMat3f("normalMatrix", normalMatrix);

    for (auto* mesh : meshes)
    {
        mesh->draw(shader);
    }
}

const std::unordered_map<std::string, GLuint>& Model::getTextureRegistry() const
{
    return textureRegistry;
}

void Model::setModelMatrix(glm::mat4& matrix)
{
    modelMatrix = matrix;
    normalMatrix = glm::inverseTranspose(glm::mat3(modelMatrix));
}

const glm::mat4& Model::getModelMatrix() const
{
    return modelMatrix;
}