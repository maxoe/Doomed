#pragma once

#include "global/config.h"
#include "renderer/mesh.h"

#include <glm/glm.hpp>
#include <string>
#include <assimp/scene.h>

class ModelLoader
{
public:
    ModelLoader() = delete;
    static Mesh* ModelLoader::load(std::string const& relModelPath);

private:
    static Mesh* processNode(const aiNode* node, const aiScene* scene);

    static void processNodeGeometryRecursively(
        const aiNode* node,
        const aiScene* scene,
        aiMatrix4x4* const accTransform,
        std::vector<VertexData>& vertices,
        std::vector<GLuint>& indices,
        std::vector<TextureData>& textures);

    static void processMeshGeometry(
        const aiMesh* mesh,
        const aiMatrix4x4* accTransform,
        std::vector<VertexData>& vertices,
        std::vector<GLuint>& indices,
        std::vector<TextureData>& textures);

    static void processSceneMaterials(const aiScene* scene, std::vector<TextureData>& textures);

    static void addTextureByType(
        const aiMaterial* material,
        const aiTextureType type,
        const char* typeName,
        std::vector<TextureData>& textures);
};
