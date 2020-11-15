#pragma once

#include "global/config.h"
#include "renderer/mesh.h"

#include <string>
#include <assimp/scene.h>

class ModelLoader
{
public:
    ModelLoader() = delete;
    static Mesh* load(std::string const& relPath);

private:
    static Mesh* processNodeGeometry(const aiNode* node, const aiScene* scene);
    static void processNodeGeometryRecursively(
        const aiNode* node,
        const aiScene* scene,
        std::vector<VertexData>& vertices,
        std::vector<GLuint>& indices);
    static void processMeshGeometry(
        const aiMesh* mesh,
        std::vector<VertexData>& vertices,
        std::vector<GLuint>& indices);
};
