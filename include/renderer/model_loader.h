#pragma once

#include "global/config.h"
#include "renderer/mesh.h"

#include <string>
#include <assimp/scene.h>

class ModelLoader
{
public:
    ModelLoader() = delete;
    static Mesh* load(std::string const& path);

private:
    static Mesh* processMeshGeometry(aiMesh* mesh);
};
