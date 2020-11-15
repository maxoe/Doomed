#include "renderer/model_loader.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "renderer/mesh.h"
#include "global/config.h"

Mesh* ModelLoader::load(std::string const& relPath)
{
    std::filesystem::path modelPath(APP_MODEL_DIR);
    modelPath = modelPath / relPath;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        modelPath.string(),
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << std::string("ERROR::ASSIMP:: ") + importer.GetErrorString() << std::endl;
        return nullptr;
    }

    return processMeshGeometry(scene->mMeshes[0]);
}

Mesh* ModelLoader::processMeshGeometry(aiMesh* mesh)
{
    std::vector<VertexData> vertices;

    for (std::size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        VertexData v = {
            {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
            {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z}};
        vertices.emplace_back(v);
    }

    std::vector<GLuint> indices;

    for (GLuint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (GLuint j = 0; j < face.mNumIndices; j++)
        {
            indices.emplace_back(face.mIndices[j]);
        }
    }

    return new Mesh(std::move(vertices), std::move(indices));
}