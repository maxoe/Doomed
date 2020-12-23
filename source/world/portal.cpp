#include "world/portal.h"

#include "core/logger.h"

#include <iostream>
#include <core/app.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
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
    const auto& scaleMatrix =
        glm::scale(glm::identity<glm::mat4>(), glm::vec3(width / size.x, height / size.y, 1.0f));

    // note that dir is inverted because given direction is the direction of travel through the
    // portal
    glm::mat4 rot = glm::lookAt(glm::vec3(0.0f), normal, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(rot * glm::vec4(pos, 1.0f)));

    portalObject->setModelMatrix(scaleMatrix * rot * trans);
}

void Portal::draw(AppShader& shader, GLuint nextFreeTextureUnit) const
{
    portalObject->draw(shader, nextFreeTextureUnit);
}

const glm::mat4& Portal::getModelMatrix() const
{
    return portalObject->getModelMatrix();
}

glm::mat4 Portal::getVirtualVPMatrix(const AppCamera& camera) const
{
    auto up = glm::vec3(0.0f, 1.0f, 0.0f);
    auto worldPos = camera.getCamWorldPos();
    auto spanTwo = glm::normalize(glm::cross(normal, up));

    const auto& portalToCam = camera.getCamWorldPos() - centerPoint;
    const auto& camDir = camera.getCameraWorldDir();

    auto scalarProjOnNormal = glm::dot(portalToCam, normal);
    auto scalarProjOnWidth = glm::dot(portalToCam, spanTwo);
    auto scalarProjOnUp = glm::dot(portalToCam, up);

    auto targetSpanTwo = glm::normalize(glm::cross(targetDir, up));

    auto dirScalarProjOnNormal = glm::dot(camDir, normal);
    auto dirScalarProjOnWidth = glm::dot(camDir, spanTwo);
    auto dirScalarProjOnUp = glm::dot(camDir, up);

    return camera.getProjection() *
           glm::lookAt(
               targetPos + scalarProjOnNormal * targetDir + scalarProjOnUp * up +
                   scalarProjOnWidth * targetSpanTwo,
               targetPos + scalarProjOnNormal * targetDir + scalarProjOnUp * up +
                   scalarProjOnWidth * targetSpanTwo + dirScalarProjOnNormal * targetDir +
                   dirScalarProjOnUp * up + dirScalarProjOnWidth * targetSpanTwo,
               glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 Portal::getTargetPosition() const
{
    return targetPos;
}

glm::vec3 Portal::getTargetDirection() const
{
    return targetDir;
}

bool Portal::collide()
{
    auto worldPos = App::getInstance()->getMaze().getCamera()->getCamWorldPos();
    auto spanTwo = glm::normalize(glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f)));
    auto toCamera = worldPos - centerPoint;
    auto scalarProjOnNormal = glm::dot(toCamera, normal);

    // bias because near plane clips away portal so stuff behind gets visible for a short time when
    // going through the portal
    const float bias = 0.15f;
    // check if we are on the correct side of the portal so we could have walked into it since the
    // last step

    if (scalarProjOnNormal <= 0.0f - bias)
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

    // we are in the bias area in front of the portal, check if we walk into the portals direction
    // since the last step
    if (lastScalarProjOnNormal < scalarProjOnNormal)
    {
        lastScalarProjOnNormal = scalarProjOnNormal;
        return true;
    }

    lastScalarProjOnNormal = scalarProjOnNormal;
    return false;
}

void Portal::teleport()
{
    lastScalarProjOnNormal = 0.0f;
    auto* cam = App::getInstance()->getMaze().getCamera();

    auto up = glm::vec3(0.0f, 1.0f, 0.0f);
    auto worldPos = cam->getCamWorldPos();
    auto spanTwo = glm::normalize(glm::cross(normal, up));
    auto toCamera = worldPos - centerPoint;

    const auto& portalToCam = cam->getCamWorldPos() - centerPoint;
    const auto& camDir = cam->getCameraWorldDir();

    auto scalarProjOnNormal = glm::dot(portalToCam, normal);
    auto scalarProjOnWidth = glm::dot(portalToCam, spanTwo);
    auto scalarProjOnUp = glm::dot(portalToCam, up);

    auto targetSpanTwo = glm::normalize(glm::cross(targetDir, up));

    auto dirScalarProjOnNormal = glm::dot(camDir, normal);
    auto dirScalarProjOnWidth = glm::dot(camDir, spanTwo);
    auto dirScalarProjOnUp = glm::dot(camDir, up);

    // TODO remove seemless?
    if (!seemless)
    {
        cam->setCamWorldPos(
            targetPos + scalarProjOnNormal * targetDir + scalarProjOnUp * up +
            scalarProjOnWidth * targetSpanTwo);
        cam->setCameraWorldDir(
            dirScalarProjOnNormal * targetDir + dirScalarProjOnUp * up +
            dirScalarProjOnWidth * targetSpanTwo);
    }
    else
    {
        cam->setCamWorldPos(
            targetPos + scalarProjOnNormal * targetDir + scalarProjOnUp * up +
            scalarProjOnWidth * targetSpanTwo);
        cam->setCameraWorldDir(
            dirScalarProjOnNormal * targetDir + dirScalarProjOnUp * up +
            dirScalarProjOnWidth * targetSpanTwo);
    }

    App::getInstance()->getMaze().setActiveNode(target);

    LOG_WORLD_INFO("Teleported from " + std::to_string(from) + " to " + std::to_string(target));
}

std::size_t Portal::getDestinationNode() const
{
    return target;
}
