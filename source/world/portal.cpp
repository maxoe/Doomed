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

    //// setup texture target
    // auto windowHeight = App::getInstance()->getHeight();
    // auto windowWidth = App::getInstance()->getWidth();
    //// fbo
    // glGenFramebuffers(1, &fbo);
    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    //// texture
    // glGenTextures(1, &texture);
    // glBindTexture(GL_TEXTURE_2D, texture);

    // glTexImage2D(
    //    GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // glGenerateMipmap(GL_TEXTURE_2D);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    // if (status != GL_FRAMEBUFFER_COMPLETE)
    //{
    //    LOG_WORLD_ERROR("Error initializing framebuffer for portal: " + std::to_string(status));
    //}

    //// restore default fbo
    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
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
    auto spanTwo = glm::normalize(glm::cross(normal, up));

    const auto& camDir = camera.getCameraWorldDir();

    auto targetSpanTwo = glm::normalize(glm::cross(targetDir, up));

    auto dirScalarProjOnNormal = glm::dot(camDir, normal);
    auto dirScalarProjOnWidth = glm::dot(camDir, spanTwo);
    auto dirScalarProjOnUp = glm::dot(camDir, up);

    return camera.getProjection() /*glm::perspective(
               glm::radians(45.0f), width / height, 0.01f, 100.0f)*/
           * glm::lookAt(
                 getVirtualCameraPosition(camera),
                 getVirtualCameraPosition(camera) + targetDir /** dirScalarProjOnNormal +
                     targetSpanTwo * dirScalarProjOnWidth + up * dirScalarProjOnUp*/
                 ,
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
    auto spanTwo = glm::normalize(glm::cross(normal, up));

    const auto& portalToCam = camera.getCamWorldPos() - centerPoint;
    auto targetSpanTwo = glm::normalize(glm::cross(targetDir, up));

    auto scalarProjOnNormal = glm::dot(portalToCam, normal);
    auto scalarProjOnWidth = glm::dot(portalToCam, spanTwo);
    auto scalarProjOnUp = glm::dot(portalToCam, up);

    return targetPos /* + scalarProjOnNormal * targetDir + scalarProjOnUp * up +
            scalarProjOnWidth * targetSpanTwo*/
        ;
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
