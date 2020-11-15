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

    return processNodeGeometry(scene->mRootNode, scene);
}

/* NO SUPPORT FOR TRANSFORMATIONS BETWEEN NODES YET */
Mesh* ModelLoader::processNodeGeometry(const aiNode* node, const aiScene* scene)
{
    std::vector<VertexData> vertices;
    std::vector<GLuint> indices;

    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMeshGeometry(mesh, vertices, indices);
    }

    for (GLuint i = 0; i < node->mNumChildren; ++i)
    {
        processNodeGeometryRecursively(node->mChildren[i], scene, vertices, indices);
    }

    return new Mesh(std::move(vertices), std::move(indices));
}

void ModelLoader::processNodeGeometryRecursively(
    const aiNode* node,
    const aiScene* scene,
    std::vector<VertexData>& vertices,
    std::vector<GLuint>& indices)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMeshGeometry(mesh, vertices, indices);
    }
}

void ModelLoader::processMeshGeometry(
    const aiMesh* mesh,
    std::vector<VertexData>& vertices,
    std::vector<GLuint>& indices)
{
    vertices.reserve(mesh->mNumVertices);

    for (std::size_t j = 0; j < mesh->mNumVertices; ++j)
    {
        VertexData v = {
            {mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z},
            {mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z}};

        vertices.emplace_back(v);
    }

    indices.reserve(mesh->mNumFaces);
    for (GLuint j = 0; j < mesh->mNumFaces; j++)
    {
        const aiFace& face = mesh->mFaces[j];

        for (GLuint k = 0; k < face.mNumIndices; k++)
        {
            indices.emplace_back(face.mIndices[k]);
        }
    }
}
