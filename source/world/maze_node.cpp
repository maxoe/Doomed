#include "world/maze_node.h"

#include <glm/ext/matrix_transform.hpp>

#include "renderer/model_loader.h"
#include "renderer/texture_loader.h"
#include "core/camera.h"

MazeNode::MazeNode()
    : shader("simple")
{
    lightWorldPos = glm::vec3(3.0f, 7.0f, 3.0);
    lightIntensity = glm::vec3(15.0f, 14.0f, 14.0f);
}

void MazeNode::draw()
{
    shader.setMat4f("VP", camera.getVP());
    shader.setVec3f("camWorldPos", camera.getCamWorldPos());

    shader.setVec3f("lightWorldPos", lightWorldPos);
    shader.setVec3f("lightIntensity", lightIntensity);

    for (auto* mesh : meshes)
    {
        mesh->draw(shader);
    }
}

void MazeNode::addObject(std::string const& relModelPath, glm::mat4& modelMatrix)
{
    meshes.emplace_back(ModelLoader::load(relModelPath));
    meshes.back()->setModelMatrix(modelMatrix);

    // load textures of mesh only if not  already loaded in maze node
    for (auto& texture : meshes.back()->getTextureData())
    {
        auto tex = loadedTextures.find(texture.path);

        if (tex == loadedTextures.end())
        {
            texture.id = TextureLoader::load(texture);
            loadedTextures.insert({texture.path, texture.id});
        }
        else
        {
            texture.id = tex->second;
        }

        texture.isValid = true;
    }
}

Camera& MazeNode::getCamera()
{
    return camera;
}