#include "world/portal.h"

#include "core/logger.h"

#include <iostream>
#include <core/app.h>
#include <glm/ext/matrix_transform.hpp>
#include <renderer/model_loader.h>

Portal::Portal(
    std::size_t fromNode,
    std::size_t toNode,
    const glm::vec3& pos,
    const glm::vec3& dir,
    float width,
    float height,
    bool seemless)
    : Portal(
          "deferred_shading/quad.obj",
          fromNode,
          toNode,
          pos,
          dir,
          width,
          height,
          glm::vec3(0.0f, 1.0f, 0.0f),
          glm::vec3(1.0f, 0.0f, 0.0f),
          seemless)
{
}

Portal::Portal(
    std::size_t fromNode,
    std::size_t toNode,
    const glm::vec3& pos,
    const glm::vec3& dir,
    float width,
    float height,
    const glm::vec3& posInTarget,
    const glm::vec3& cameraDirectionInTarget,
    bool seemless)
    : Portal(
          "deferred_shading/quad.obj",
          fromNode,
          toNode,
          pos,
          dir,
          width,
          height,
          posInTarget,
          cameraDirectionInTarget,
          seemless)
{
}

Portal::Portal(
    const std::string& relModelPath,
    std::size_t fromNode,
    std::size_t toNode,
    const glm::vec3& pos,
    const glm::vec3& dir,
    float width,
    float height,
    bool seemless)

    : Portal(
          relModelPath,
          fromNode,
          toNode,
          pos,
          dir,
          width,
          height,
          glm::vec3(0.0f, 1.0f, 0.0f),
          glm::vec3(1.0f, 0.0f, 0.0f),
          seemless)
{
}

Portal::Portal(
    const std::string& relModelPath,
    std::size_t fromNode,
    std::size_t toNode,
    const glm::vec3& pos,
    const glm::vec3& dir,
    float width,
    float height,
    const glm::vec3& posInTarget,
    const glm::vec3& cameraDirectionInTarget,
    bool seemless)
    : portalObject(ModelLoader::load(relModelPath))
    , from(fromNode)
    , target(toNode)
    , targetPos(posInTarget)
    , targetDir(cameraDirectionInTarget)
    , normal(glm::normalize(dir))
    , centerPoint(pos)
    , width(width)
    , height(height)
    , seemless(seemless)
{
    const auto& size = portalObject->getWorldSize();

    // note that size.z is 0 because the quad is 2-dimensional
    const auto& matrix =
        glm::scale(glm::identity<glm::mat4>(), glm::vec3(width / size.x, height / size.y, 1.0f));

    portalObject->setModelMatrix(glm::lookAt(pos, pos + dir, glm::vec3(0.0f, 1.0f, 0.0f)) * matrix);
}

void Portal::draw(AppShader& shader, GLuint nextFreeTextureUnit) const
{
    portalObject->draw(shader, nextFreeTextureUnit);
}

const glm::mat4& Portal::getModelMatrix() const
{
    return portalObject->getModelMatrix();
}

bool Portal::collide()
{
    auto worldPos = App::getInstance()->getMaze().getCamera()->getCamWorldPos();
    auto spanTwo = glm::normalize(glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f)));
    auto toCamera = worldPos - centerPoint;
    auto scalarProjOnNormal = glm::dot(toCamera, normal);

    // check if we are on the correct side of the portal so we could have walked into it since the
    // last step
    if (lastScalarProjOnNormal >= 0.0f)
    {
        lastScalarProjOnNormal = scalarProjOnNormal;
        return false;
    }

    // check if camera is below oder beneath the portal
    if (glm::abs(toCamera.y) > height / 2.0f)
    {
        lastScalarProjOnNormal = scalarProjOnNormal;
        return false;
    }

    // check if we are next to the portal

    auto scalarProjOnWidth = glm::dot(toCamera, spanTwo);

    if (glm::abs(scalarProjOnWidth) > width / 2.0f)
    {
        lastScalarProjOnNormal = scalarProjOnNormal;
        return false;
    }

    // we are in front of the portal, check if we walked through the portal since the last step
    if (scalarProjOnNormal > 0.0f && lastScalarProjOnNormal < 0.0f)
    {
        lastScalarProjOnNormal = scalarProjOnNormal;
        return true;
    }

    return false;
}

void Portal::teleport()
{
    lastScalarProjOnNormal = 0.0f;
    auto* cam = App::getInstance()->getMaze().getCamera();

    if (!seemless)
    {
        cam->setCamWorldPos(targetPos);
        cam->setYaw(-0.85f);
        cam->setPitch(14.18f);
    }
    else
    {
        cam->setCamWorldPos(targetPos + (cam->getCamWorldPos() - centerPoint));
    }

    App::getInstance()->getMaze().setActiveNode(target);

    LOG_WORLD_INFO("Teleported from " + std::to_string(from) + " to " + std::to_string(target));
}

const std::size_t Portal::getDestinationNode() const
{
    return target;
}
