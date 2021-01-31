#include "world/maze_node.h"

#include <iostream>
#include <core/app.h>
#include <glm/ext/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"

#include "renderer/model_loader.h"
#include "renderer/point_light.h"
#include "core/app_camera.h"
#include "core/logger.h"
#include "world/geometry_builder.h"

MazeNode::MazeNode()
{
}

void MazeNode::setLightUniforms(const AppShader& shader) const
{
    // TODO KICK THIS STUPID FUNCTION OUT OF THIS UNIVERSE
    /* shader.setVec3f("ambient", ambient);

     if (hasDirectionalLight)
     {
         shader.setDirectionalLight(directionalLightDir, directionalLightIntensity);
     }*/

    // TODO MOVE THIS PROPERLY
    /*for (std::size_t i = 0; i < pointLights.size(); ++i)
    {
        shader.setPointLight(pointLights[i], i);
    }

    shader.setInt(std::string("pointLightCount"), pointLights.size());*/
}

void MazeNode::draw(AppShader& shader, GLuint nextFreeTextureUnit) const
{
    for (const auto* model : models)
    {
        model->draw(shader, nextFreeTextureUnit);
    }
}

void MazeNode::drawPortals(AppShader& shader, GLuint nextFreeTextureUnit) const
{
    for (const auto* portal : portals)
    {
        portal->draw(shader, nextFreeTextureUnit);
    }
}

void MazeNode::update()
{
    for (auto& l : pointLights)
    {
        l.update();
    }

    for (auto* portal : portals)
    {
        if (portal->collide())
        {
            portal->teleport();
            break;
        }
    }
}

MazeNode* MazeNode::addPortal(const glm::vec3& pos, const glm::vec3& dir, float width, float height)
{
    portals.emplace_back(new Portal(
        App::getInstance()->getMaze().getNodeIndex(this),
        App::getInstance()->getMaze().getNodeIndex(this),
        pos,
        dir,
        width,
        height,
        false));

    return this;
}

MazeNode* MazeNode::addPortal(
    const glm::vec3& pos,
    const glm::vec3& dir,
    float width,
    float height,
    const glm::vec3& posInTarget,
    const glm::vec3& cameraDirectionInTarget)
{
    portals.emplace_back(new Portal(
        App::getInstance()->getMaze().getNodeIndex(this),
        App::getInstance()->getMaze().getNodeIndex(this),
        pos,
        dir,
        width,
        height,
        posInTarget,
        cameraDirectionInTarget,
        false));

    return this;
}

MazeNode* MazeNode::addPortal(
    std::size_t destination,
    const glm::vec3& pos,
    const glm::vec3& dir,
    float width,
    float height,
    bool seemless)
{
    portals.emplace_back(new Portal(
        App::getInstance()->getMaze().getNodeIndex(this),
        destination,
        pos,
        dir,
        width,
        height,
        seemless));

    return this;
}

MazeNode* MazeNode::addPortal(
    std::size_t destination,
    const glm::vec3& pos,
    const glm::vec3& dir,
    float width,
    float height,
    const glm::vec3& posInTarget,
    const glm::vec3& cameraDirectionInTarget,
    bool seemless)
{
    portals.emplace_back(new Portal(
        App::getInstance()->getMaze().getNodeIndex(this),
        destination,
        pos,
        dir,
        width,
        height,
        posInTarget,
        cameraDirectionInTarget,
        seemless));

    return this;
}

MazeNode* MazeNode::addPortal(
    const std::string& relModelPath,
    std::size_t destination,
    const glm::vec3& pos,
    const glm::vec3& dir,
    float width,
    float height,
    bool seemless)
{
    portals.emplace_back(new Portal(
        relModelPath,
        App::getInstance()->getMaze().getNodeIndex(this),
        destination,
        pos,
        dir,
        width,
        height,
        seemless));

    return this;
}

MazeNode* MazeNode::addModel(const std::string& relModelPath, const glm::mat4& modelMatrix)
{
    models.emplace_back(ModelLoader::load(relModelPath));
    models.back()->setModelMatrix(modelMatrix);

    return this;
}

MazeNode* MazeNode::addModel(const std::string& relModelPath, float maxDimensionLength)
{
    models.emplace_back(ModelLoader::load(relModelPath));
    models.back()->resize(maxDimensionLength);

    return this;
}

MazeNode* MazeNode::addModel(const std::string& relModelPath)
{
    glm::mat4 modelMatrix = glm::identity<glm::mat4>();
    addModel(relModelPath, modelMatrix);

    return this;
}

MazeNode* MazeNode::addOpenBox(
    std::size_t length,
    std::size_t width,
    std::size_t height,
    const glm::vec3& dir,
    const glm::vec3& sndDir,
    const glm::vec3& leftBottomCorner,
    const glm::vec3& color,
    const glm::vec3& sndColor)
{
    glm::vec3 thirdDir = glm::cross(sndDir, dir);
    const float cubeSize = 2.0f;

    // floor
    addWall(length, width, dir, sndDir, leftBottomCorner, color, sndColor);
    // headwall
    addWall(length, height, dir, thirdDir, leftBottomCorner, color, sndColor);
    // left
    addWall(width, height, sndDir, thirdDir, leftBottomCorner, color, sndColor);

    addWall(
        width,
        height,
        sndDir,
        thirdDir,
        leftBottomCorner + dir * static_cast<float>(length) * cubeSize,
        color,
        sndColor);
    addWall(
        length,
        height,
        dir,
        thirdDir,
        leftBottomCorner + sndDir * static_cast<float>(width) * cubeSize,
        color,
        sndColor);

    return this;
}

MazeNode* MazeNode::addBox(
    std::size_t length,
    std::size_t width,
    std::size_t height,
    const glm::vec3& dir,
    const glm::vec3& sndDir,
    const glm::vec3& leftBottomCorner,
    const glm::vec3& color,
    const glm::vec3& sndColor)
{
    glm::vec3 thirdDir = glm::cross(sndDir, dir);
    const float cubeSize = 2.0f;

    addOpenBox(length, width, height, dir, sndDir, leftBottomCorner, color, sndColor);

    addWall(
        length,
        width,
        dir,
        sndDir,
        leftBottomCorner + thirdDir * static_cast<float>(height) * cubeSize,
        color,
        sndColor);

    return this;
}

MazeNode* MazeNode::addWall(
    std::size_t length,
    std::size_t width,
    const glm::vec3& dir,
    const glm::vec3& sndDir,
    const glm::vec3& leftBottomCorner,
    const glm::vec3& color,
    const glm::vec3& sndColor)
{
    int count = 0;

    for (int j = 0; j < length; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            addCube(
                leftBottomCorner +
                    2.0f * (static_cast<float>(j) * dir + static_cast<float>(i) * sndDir),
                1.0f,
                (i + j) % 2 == 0 ? sndColor : color);
            ++count;
        }

        ++count;
    }

    return this;
}

MazeNode* MazeNode::addCube(const glm::vec3& pos, float size, const glm::vec3& color)
{
    glm::mat4 modelMatrix = glm::translate(glm::identity<glm::mat4>(), pos);
    addModel("cube.obj", modelMatrix);
    models.back()->resize(size);
    models.back()->setSingleColor(color);

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

MazeNode*
MazeNode::addPointLight(const glm::vec3& pos, const glm::vec3& intensity, float dist, bool shadows)
{
    pointLights.emplace_back(PointLight(pos, intensity, dist, shadows));

    return this;
}

MazeNode* MazeNode::addPointLight(
    const glm::vec3& pos,
    const glm::vec3& intensity,
    float dist,
    bool shadows,
    const std::vector<glm::vec4>& initKeyframes)
{
    pointLights.emplace_back(PointLight(pos, intensity, dist, shadows, initKeyframes));

    return this;
}

glm::vec3 MazeNode::getDirectionalLightDirection() const
{
    return directionalLightDir;
}

glm::vec3 MazeNode::getDirectionalLightIntensity() const
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

std::vector<Portal*>& MazeNode::getPortals()
{
    return portals;
}

std::vector<PointLight>& MazeNode::getPointLights()
{
    return pointLights;
}

bool MazeNode::getHasDirectionalLight() const
{
    return hasDirectionalLight;
}

bool MazeNode::getIsVisible() const
{
    return isVisible;
}

void MazeNode::setIsVisible(bool visible)
{
    isVisible = visible;
}
