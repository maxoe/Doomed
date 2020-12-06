#include "renderer/model_loader.h"

#include <vector>
#include <filesystem>
#include <iostream>

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "renderer/mesh.h"
#include "global/config.h"
#include "renderer/model.h"
#include "core/logger.h"

Model* ModelLoader::load(std::string const& relModelPath)
{
    std::filesystem::path cleanPath(relModelPath);
    cleanPath = cleanPath.make_preferred();

    std::filesystem::path modelPath(APP_MODEL_DIR);
    modelPath = modelPath.make_preferred() / cleanPath;

    std::filesystem::path texturePath(APP_TEXTURE_DIR);
    std::string relTextureDir =
        (texturePath.make_preferred() / cleanPath).remove_filename().string();

    Assimp::Importer importer;

    const unsigned pFlags = aiProcess_Triangulate | aiProcess_FlipUVs |
                            aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices |
                            aiProcess_GenNormals;
    const aiScene* scene = importer.ReadFile(modelPath.string(), pFlags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_LOADER_ERROR(std::string("ERROR::ASSIMP:: ") + importer.GetErrorString());
        return new Model();
    }

    std::vector<Mesh*> meshes = processScene(scene->mRootNode, scene);

    return new Model(meshes, modelPath.string(), relTextureDir);
}

std::vector<Mesh*> ModelLoader::processScene(const aiNode* node, const aiScene* scene)
{
    std::vector<Mesh*> meshes;
    aiMatrix4x4 accTransform = aiMatrix4x4();

    meshes.reserve(node->mNumMeshes);
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.emplace_back(processMesh(mesh, &accTransform, scene));
    }

    for (GLuint i = 0; i < node->mNumChildren; ++i)
    {
        processNodeGeometryRecursively(node->mChildren[i], scene, &accTransform, meshes);
    }

    return meshes;
}

void ModelLoader::processNodeGeometryRecursively(
    const aiNode* node,
    const aiScene* scene,
    aiMatrix4x4* const accTransform,
    std::vector<Mesh*>& meshes)
{
    meshes.reserve(node->mNumMeshes);

    for (GLuint i = 0; i < node->mNumMeshes; ++i)
    {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.emplace_back(processMesh(mesh, accTransform, scene));
    }

    for (GLuint i = 0; i < node->mNumChildren; ++i)
    {
        processNodeGeometryRecursively(node->mChildren[i], scene, accTransform, meshes);
    }
}

Mesh* ModelLoader::processMesh(
    const aiMesh* mesh,
    const aiMatrix4x4* transform,
    const aiScene* scene)
{
    std::vector<VertexData> vertices;
    std::vector<GLuint> indices;

    // vertices
    vertices.reserve(mesh->mNumVertices);
    /*aiQuaternion rotation;
     aiVector3D trash;
     transform->Decompose(trash, rotation, trash);*/

    for (std::size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        const aiVector3D positionTransformed = mesh->mVertices[i];
        const aiVector3D normalTransformed = mesh->mNormals[i];

        // textures coords if exist
        glm::vec2 texCoords;
        if (mesh->mTextureCoords[0])
        {
            texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
        {
            texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.emplace_back(VertexData{
            {positionTransformed.x, positionTransformed.y, positionTransformed.z},
            {normalTransformed.x, normalTransformed.y, normalTransformed.z},
            texCoords});
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

    // materials if exist
    auto textures = processMeshMaterials(mesh, scene);

    return new Mesh(vertices, indices, textures);
}

std::vector<TextureData> ModelLoader::processMeshMaterials(const aiMesh* mesh, const aiScene* scene)
{
    std::vector<TextureData> textures;

    if (mesh->mMaterialIndex > 0)  // TODO NOT SURE HOW TO CHECK PROPERLY FOR MATERIALS
    {
        const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        addTextureByType(material, aiTextureType_DIFFUSE, "textureDiffuse", textures);
        addTextureByType(material, aiTextureType_SPECULAR, "textureSpecular", textures);
    }

    return textures;
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
        TextureData texture{0, str.C_Str(), typeName, false};
        textures.emplace_back(texture);
    }
}
