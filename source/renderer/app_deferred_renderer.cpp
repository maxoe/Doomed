#include "renderer/app_deferred_renderer.h"

#include <iostream>

#include "core/app.h"
#include "core/logger.h"
#include "world/maze.h"
#include <renderer/model_loader.h>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_inverse.inl>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

// shader will be default initialized and fall back to "default"

AppDeferredRenderer::AppDeferredRenderer()
{
    auto width = 0;
    auto height = 0;

    glfwGetFramebufferSize(App::getInstance()->getWindow(), &width, &height);

    gBuffer = std::make_shared<AppGBuffer>(width, height);
    boundingSphere = ModelLoader::load("deferred_shading/sphere.obj");
    quad = ModelLoader::load("deferred_shading/quad.obj");
    quad->setModelMatrix(glm::identity<glm::mat4>());

    LOG_RENDERER_INFO("Using " + getTypeNameStatic() + " renderer");
}

void AppDeferredRenderer::render(Maze* maze)
{
    // 0 geometry 1 point light 2 directional light
    const auto& geometryShader = shader[0];
    const auto& pointLightShader = shader[1];
    const auto& dirLightShader = shader[2];

    auto width = 0;
    auto height = 0;
    glfwGetFramebufferSize(App::getInstance()->getWindow(), &width, &height);

    // geometry pass
    geometryShader.use();
    gBuffer->bindForWriting();

    // only geometry pass writes to depth buffer and does depth tests
    glDepthMask(GL_TRUE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    const auto& c = maze->getCamera();
    pointLightShader.setMat4f("VP", c.getVP());
    pointLightShader.setVec3f("camWorldPos", c.getCamWorldPos());

    for (auto* node : maze->getNodes())
    {
        node->setLightUniforms(pointLightShader);
        node->draw(geometryShader);
    }

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    // setup for light pass
    // will blend multiple light passes
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    gBuffer->bindForReading();
    glClear(GL_COLOR_BUFFER_BIT);

    // point light pass
    pointLightShader.use();
    gBuffer->setUniforms(pointLightShader);
    pointLightShader.setVec2f("screenSize", width, height);

    // camera uniforms
    pointLightShader.setMat4f("VP", c.getVP());
    pointLightShader.setVec3f("camWorldPos", c.getCamWorldPos());

    // need light position and distance
    for (const auto& l : maze->getActiveNode()->getPointLights())
    {
        // only one light per draw call so index is 0
        pointLightShader.setPointLight(l, 0);
        pointLightShader.setInt(std::string("pointLightCount"), 1);

        float lightDist = l.getDist();
        glm::mat4 modelMatrix = glm::scale(glm::vec3(lightDist));

        boundingSphere->setModelMatrix(modelMatrix);
        boundingSphere->draw(pointLightShader);
    }

    // directional light pass
    const auto* n = maze->getActiveNode();
    if (n->getHasDirectionalLight())
    {
        dirLightShader.use();

        gBuffer->setUniforms(dirLightShader);
        dirLightShader.setVec2f("screenSize", width, height);
        dirLightShader.setVec3f("camWorldPos", c.getCamWorldPos());
        dirLightShader.setDirectionalLight(
            n->getDirectionalLightDirection(), n->getDirectionalLightIntensity());

        quad->draw(pointLightShader);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::string AppDeferredRenderer::getTypeName() const
{
    return getTypeNameStatic();
}