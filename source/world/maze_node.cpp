#include "world/maze_node.h"
#include "renderer/model_loader.h"
#include "renderer/texture_loader.h"

#include <iostream>

void MazeNode::draw(AppShader& shader)
{
    for (auto* mesh : meshes)
    {
        mesh->draw(shader);
    }
}

void MazeNode::addObject(std::string const& relModelPath)
{
    meshes.emplace_back(ModelLoader::load(relModelPath));

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
