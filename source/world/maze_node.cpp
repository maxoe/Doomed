#include "world/maze_node.h"

#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"

#include "renderer/model_loader.h"
#include "renderer/point_light.h"
#include "core/camera.h"
#include "core/logger.h"

MazeNode::MazeNode()
    : ambient(glm::vec3(0.f))
    , directionalLightDir(0.f)
    , directionalLightIntensity(glm::vec3(0.f))
{
}

void MazeNode::setLightUniforms(const AppShader& shader) const
{
    shader.setVec3f("ambient", ambient);

    if (hasDirectionalLight)
    {
        shader.setDirectionalLight(directionalLightDir, directionalLightIntensity);
    }

    for (std::size_t i = 0; i < pointLights.size(); ++i)
    {
        shader.setPointLight(pointLights[i], i);
    }

    shader.setInt(std::string("pointLightCount"), pointLights.size());
}

void MazeNode::draw(const AppShader& shader)
{
    for (const auto* model : models)
    {
        model->draw(shader);
    }
}

MazeNode* MazeNode::addModel(const std::string& relModelPath, const glm::mat4& modelMatrix)
{
    models.emplace_back(ModelLoader::load(relModelPath));
    models.back()->setModelMatrix(modelMatrix);

    return this;
}

MazeNode* MazeNode::addModel(const std::string& relModelPath)
{
    glm::mat4 modelMatrix = glm::identity<glm::mat4>();
    addModel(relModelPath, modelMatrix);

    return this;
}

/*
 * Adds a new model to the node relative to the last model in the models member according to the
 * attachment point. If preserveRoation is true, then the new model will have the same rotation as
 * the previous model.
 *
 */
MazeNode* MazeNode::attachModelToLast(
    const std::string& relModelPath,
    AttachmentPoint ap,
    bool preserveRotation)
{
    const auto* lastModel = models.back();

    if (preserveRotation)
    {
        addModel(relModelPath, lastModel->getModelMatrix());
    }
    else
    {
        const auto& translation = lastModel->getTranslation();
        addModel(relModelPath, glm::translate(glm::identity<glm::mat4>(), translation));
    }

    auto* newModel = models.back();

    // model is now positioned on top of the previously added model
    // translate according to the attachment point
    newModel->addModelTransformation(
        glm::translate(glm::identity<glm::mat4>(), calcAttachmentOffset(lastModel, newModel, ap)));

    // preserve scaling
    newModel->addModelTransformation(glm::scale(
        glm::identity<glm::mat4>(), lastModel->getWorldSize() / lastModel->getObjectSize()));

    return this;
}

/*
 * Calculate offset to translate the model newModel from a perfectly overlapping position to a
 * position where it connects to oldModel as specified by the attachment point.
 * Example: PosX will translate newModel in a way that newModel will follow oldModel in positive x
 * direction.
 */
glm::vec3 MazeNode::calcAttachmentOffset(
    const Model* oldModel,
    const Model* newModel,
    AttachmentPoint ap) const
{
    switch (ap)
    {
        case AttachmentPoint::PosX:
            return glm::vec3(
                glm::abs(oldModel->getObjectMaxValues().x) +
                    glm::abs(newModel->getObjectMinValues().x),
                0.0f,
                0.0f);
        case AttachmentPoint::NegX:
            return glm::vec3(
                -(glm::abs(newModel->getObjectMaxValues().x) +
                  glm::abs(oldModel->getObjectMinValues().x)),
                0.0f,
                0.0f);
        case AttachmentPoint::PosY:
            return glm::vec3(
                0.0f,
                glm::abs(oldModel->getObjectMaxValues().y) +
                    glm::abs(newModel->getObjectMinValues().y),
                0.0f);
        case AttachmentPoint::NegY:
            return glm::vec3(
                0.0f,
                -(glm::abs(newModel->getObjectMaxValues().y) +
                  glm::abs(oldModel->getObjectMinValues().y)),
                0.0f);
        case AttachmentPoint::PosZ:
            return glm::vec3(
                0.0f,
                0.0f,
                glm::abs(oldModel->getObjectMaxValues().z) +
                    glm::abs(newModel->getObjectMinValues().z));
        case AttachmentPoint::NegZ:
            return glm::vec3(
                0.0f,
                0.0f,
                -(glm::abs(newModel->getObjectMaxValues().z) +
                  glm::abs(oldModel->getObjectMinValues().z)));
    }

    LOG_WORLD_ERROR("Invalid attachment point");
    return glm::vec3();
}

MazeNode* MazeNode::addPointLight(const glm::vec3& pos, const glm::vec3& intensity, float dist)
{
    pointLights.emplace_back(PointLight(pos, intensity, dist));

    return this;
}

glm::vec3 MazeNode::getDirectionalLightDirection() const
{
    return directionalLightDir;
}

[[nodiscard]] glm::vec3 MazeNode::getDirectionalLightIntensity() const
{
    return directionalLightIntensity;
}

MazeNode* MazeNode::setDirectionalLight(const glm::vec3& dir, const glm::vec3& intensity)
{
    directionalLightDir = dir;
    directionalLightIntensity = intensity;
    hasDirectionalLight = true;

    return this;
}

const std::vector<PointLight>& MazeNode::getPointLights() const
{
    return pointLights;
}

bool MazeNode::getHasDirectionalLight() const
{
    return hasDirectionalLight;
}
