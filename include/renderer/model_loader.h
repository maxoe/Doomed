#pragma once

#include "renderer/mesh.h"

#include <string>
#include <assimp/scene.h>

#include "model.h"

class ModelLoader
{
public:
    ModelLoader() = delete;
    static Model* load(std::string const& relModelPath);

private:
    static const std::string& getModelPath(const std::string& path);
    static const std::string& getTextureDir(const std::string& path);
    static std::vector<Mesh*> processScene(const aiNode* node, const aiScene* scene);

    static void processNodeGeometryRecursively(
        const aiNode* node,
        const aiScene* scene,
        aiMatrix4x4* const accTransform,
        std::vector<Mesh*>& meshes);

    static Mesh*
    processMesh(const aiMesh* mesh, const aiMatrix4x4* accTransform, const aiScene* scene);

    static std::vector<TextureData> processMeshMaterials(const aiMesh* mesh, const aiScene* scene);

    static void addTextureByType(
        const aiMaterial* material,
        const aiTextureType type,
        const char* typeName,
        std::vector<TextureData>& textures);
};
