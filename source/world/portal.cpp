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
    : /*fbo(0)
    , texture(0)
    , */
    portalObject(ModelLoader::load(relModelPath))
    , from(fromNode)
    , target(toNode)
    , targetPos(posInTarget)
    , targetDir(glm::normalize(cameraDirectionInTarget) * glm::sign(cameraDirectionInTarget))
    , normal(glm::normalize(dir) * glm::sign(dir))
    , centerPoint(pos)
    , width(width)
    , height(height)
    , seemless(seemless)
{
    const auto& size = portalObject->getWorldSize();
    /*std::cout << "Portal with height " << height << " width " << width << " and dir " << normal.x
              << " " << normal.y << " " << normal.z << std::endl;*/

    // note that dir is inverted because given direction is the direction of travel through the
    // portal
    glm::mat4 rot = glm::lookAt(glm::vec3(0.0f), -normal, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(rot * glm::vec4(pos, 1.0f)));

    // note that size.z is 0 because the quad is 2-dimensional
    const auto& scaleMatrix =
        glm::scale(glm::vec3(glm::vec4(width / size.x, height / size.y, 1.0f, 0.0f)));
    /*std::cout << glm::vec3(glm::vec4(width / size.x, height / size.y, 1.0f, 0.0f)).x << " "
              << glm::vec3(glm::vec4(width / size.x, height / size.y, 1.0f, 0.0f)).y << " "
              << glm::vec3(glm::vec4(width / size.x, height / size.y, 1.0f, 0.0f)).z << " "
              << std::endl;
    std::cout << glm::vec3(rot * glm::vec4(width / size.x, height / size.y, 1.0f, 0.0f)).x << " "
              << glm::vec3(rot * glm::vec4(width / size.x, height / size.y, 1.0f, 0.0f)).y << " "
              << glm::vec3(rot * glm::vec4(width / size.x, height / size.y, 1.0f, 0.0f)).z << " "
              << std::endl;
    std::cout << std::endl;*/
    portalObject->setModelMatrix(scaleMatrix * rot * trans);

    /*std::cout << "Portal has actual x " << portalObject->getWorldSize().x << ", y "
              << portalObject->getWorldSize().y << " and z " << portalObject->getWorldSize().z
              << std::endl;*/

    // TODO remove width hack
    auto spanTwo = glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f));
    this->width = glm::abs(glm::dot(portalObject->getWorldSize(), spanTwo));
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
    auto spanTwo = glm::cross(normal, up);

    const auto& camDir = camera.getCameraWorldDir();

    auto targetSpanTwo = glm::cross(targetDir, up);

    auto dirScalarProjOnNormal = glm::dot(camDir, normal);
    auto dirScalarProjOnWidth = glm::dot(camDir, spanTwo);
    auto dirScalarProjOnUp = glm::dot(camDir, up);

    return camera.getProjection() *
           glm::lookAt(
               getVirtualCameraPosition(camera),
               getVirtualCameraPosition(camera) + getVirtualCameraDirection(camera),
               glm::vec3(0.0f, 1.0f, 0.0f));
}

float Portal::getWidth() const
{
    return width;
}

float Portal::getHeight() const
{
    return height;
}

glm::vec2 Portal::getSize() const
{
    return glm::vec2(portalObject->getObjectSize());
}

glm::vec3 Portal::getVirtualCameraPosition(const AppCamera& camera) const
{
    auto up = glm::vec3(0.0f, 1.0f, 0.0f);
    auto spanTwo = glm::cross(normal, up);

    const auto& portalToCam = camera.getCamWorldPos() - centerPoint;
    auto targetSpanTwo = glm::cross(targetDir, up);

    auto scalarProjOnNormal = glm::dot(portalToCam, normal);
    auto scalarProjOnWidth = glm::dot(portalToCam, spanTwo);
    auto scalarProjOnUp = glm::dot(portalToCam, up);

    return targetPos + scalarProjOnNormal * targetDir + scalarProjOnUp * up +
           scalarProjOnWidth * targetSpanTwo;
}

glm::vec3 Portal::getTargetPosition() const
{
    return targetPos;
}

glm::vec3 Portal::getTargetDirection() const
{
    return targetDir;
}
glm::vec3 Portal::getVirtualCameraDirection(const AppCamera& camera) const
{
    auto up = glm::vec3(0.0f, 1.0f, 0.0f);
    auto spanTwo = glm::cross(normal, up);

    const auto& camDir = camera.getCameraWorldDir();

    auto targetSpanTwo = glm::cross(targetDir, up);

    auto dirScalarProjOnNormal = glm::dot(camDir, normal);
    auto dirScalarProjOnWidth = glm::dot(camDir, spanTwo);
    auto dirScalarProjOnUp = glm::dot(camDir, up);

    return targetDir * dirScalarProjOnNormal + targetSpanTwo * dirScalarProjOnWidth +
           up * dirScalarProjOnUp;
}

bool Portal::collide()
{
    auto worldPos = App::getInstance()->getMaze().getCamera()->getCamWorldPos();
    auto spanTwo = glm::cross(normal, glm::vec3(0.0f, 1.0f, 0.0f));
    auto toCamera = worldPos - centerPoint;
    auto scalarProjOnNormal = glm::dot(toCamera, normal);

    float worldHeight = portalObject->getWorldSize().y;
    float worldWidth = portalObject->getWorldSize().z;

    // bias because near plane clips away portal so stuff behind gets visible for a short time
    // when going through the portal
    const float bias = 0.15f;
    // check if we are on the correct side of the portal so we could have walked into it since
    // the last step
    // std::cout << "old scalar proj on normal is " << lastScalarProjOnNormal << std::endl;
    // std::cout << "scalar proj on normal is " << scalarProjOnNormal << std::endl;

    if (scalarProjOnNormal <= 0.0f - bias)
    {
        // std::cout << "outside normal" << std::endl;
        lastScalarProjOnNormal = scalarProjOnNormal;
        return false;
    }

    // check if camera is below oder beneath the portal
    if (glm::abs(toCamera.y) > height / 2.0f)
    {
        // std::cout << "outside height" << std::endl;
        lastScalarProjOnNormal = scalarProjOnNormal;
        return false;
    }

    // check if we are next to the portal

    auto scalarProjOnWidth = glm::dot(toCamera, spanTwo);

    if (glm::abs(scalarProjOnWidth) > width / 2.0f)
    {
        // std::cout << "outside width: " << glm::abs(scalarProjOnWidth) << " > " << width / 2.0
        //<< std::endl;
        // std::cout << "length of diff: " << glm::length(toCamera) << std::endl;

        lastScalarProjOnNormal = scalarProjOnNormal;
        return false;
    }

    // std::cout << "can teleport" << std::endl;

    // we are in the bias area in front of the portal, check if we walk into the portals
    // direction since the last step
    if (/*lastScalarProjOnNormal < scalarProjOnNormal ||*/
        (lastScalarProjOnNormal < 0 && scalarProjOnNormal > 0 - bias))
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
    auto spanTwo = glm::cross(normal, up);
    auto toCamera = worldPos - centerPoint;

    const auto& portalToCam = cam->getCamWorldPos() - centerPoint;
    const auto& camDir = cam->getCameraWorldDir();

    auto scalarProjOnNormal = glm::dot(portalToCam, normal);
    auto scalarProjOnWidth = glm::dot(portalToCam, spanTwo);
    auto scalarProjOnUp = glm::dot(portalToCam, up);

    auto targetSpanTwo = glm::cross(targetDir, up);

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
