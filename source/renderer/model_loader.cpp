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

Mesh* ModelLoader::load(std::string const& relModelPath)
{
    std::filesystem::path modelPath(APP_MODEL_DIR);
    modelPath = modelPath / relModelPath;

    Assimp::Importer importer;

    const aiScene* scene;
    unsigned pFlags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices |
                      aiProcess_PreTransformVertices;
    scene = importer.ReadFile(modelPath.string(), pFlags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << std::string("ERROR::ASSIMP:: ") + importer.GetErrorString() << std::endl;
        return nullptr;
    }

    return processNode(scene->mRootNode, scene);
}

Mesh* ModelLoader::processNode(const aiNode* node, const aiScene* scene)
{
    std::vector<VertexData> vertices;
    std::vector<GLuint> indices;
    std::vector<TextureData> textures;
    aiMatrix4x4 accTransform = aiMatrix4x4();

    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMeshGeometry(mesh, &accTransform, vertices, indices, textures);
    }

    for (GLuint i = 0; i < node->mNumChildren; ++i)
    {
        processNodeGeometryRecursively(
            node->mChildren[i], scene, &accTransform, vertices, indices, textures);
    }

    processSceneMaterials(scene, textures);

    return new Mesh(std::move(vertices), std::move(indices), std::move((textures)));
}

void ModelLoader::processNodeGeometryRecursively(
    const aiNode* node,
    const aiScene* scene,
    aiMatrix4x4* const accTransform,
    std::vector<VertexData>& vertices,
    std::vector<GLuint>& indices,
    std::vector<TextureData>& textures)
{
    for (GLuint i = 0; i < node->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMeshGeometry(mesh, accTransform, vertices, indices, textures);
    }

    for (GLuint i = 0; i < node->mNumChildren; ++i)
    {
        processNodeGeometryRecursively(
            node->mChildren[i], scene, accTransform, vertices, indices, textures);
    }
}

void ModelLoader::processMeshGeometry(
    const aiMesh* mesh,
    const aiMatrix4x4* transform,
    std::vector<VertexData>& vertices,
    std::vector<GLuint>& indices,
    std::vector<TextureData>& textures)
{
    // vertices
    vertices.reserve(mesh->mNumVertices);
    aiQuaternion rotation;
    aiVector3D trash;
    transform->Decompose(trash, rotation, trash);

    for (std::size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        const aiVector3D positionTransformed = *transform * mesh->mVertices[i];
        const aiVector3D normalTransformed = rotation.Rotate(mesh->mNormals[i]);

        VertexData v = {
            {positionTransformed.x, positionTransformed.y, positionTransformed.z},
            {normalTransformed.x, normalTransformed.y, normalTransformed.z}};

        // check first if mesh contains textures
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            v.texCoords = vec;
        }
        else
        {
            v.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.emplace_back(v);
    }

    // triangle indices
    indices.reserve(mesh->mNumFaces);
    for (GLuint i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];

        for (GLuint j = 0; j < face.mNumIndices; j++)
        {
            indices.emplace_back(face.mIndices[j]);
        }
    }
}

void ModelLoader::processSceneMaterials(const aiScene* scene, std::vector<TextureData>& textures)
{
    if (scene->HasMaterials())
    {
        for (GLuint i = 0; i < scene->mNumMaterials; i++)
        {
            const aiMaterial* material = scene->mMaterials[i];

            addTextureByType(material, aiTextureType_DIFFUSE, "texture_diffuse", textures);
            addTextureByType(material, aiTextureType_SPECULAR, "texture_specular", textures);
        }
    }
}

void ModelLoader::addTextureByType(
    const aiMaterial* material,
    const aiTextureType type,
    const char* typeName,
    std::vector<TextureData>& textures)
{
    for (GLuint i = 0; i < material->GetTextureCount(type); ++i)
    {
        aiString str;
        material->GetTexture(type, i, &str);
        TextureData texture{0, typeName, str.C_Str(), false};
        textures.emplace_back(texture);
    }
}
