#include "renderer/app_deferred_renderer.h"
#include "renderer/app_renderer.h"

#include <iostream>

#include "core/app.h"
#include "core/logger.h"
#include "world/maze.h"
#include <renderer/model_loader.h>

#include <GLFW/glfw3.h>

// shader will be default initialized and fall back to "default"

AppDeferredRenderer::AppDeferredRenderer()
{
    auto width = 0;
    auto height = 0;

    glfwGetFramebufferSize(App::getInstance()->getWindow(), &width, &height);

    gBuffer = std::make_shared<AppGBuffer>(width, height);
    boundingSphere = ModelLoader::load("sphere.obj");

    LOG_RENDERER_INFO("Using " + getTypeNameStatic() + " renderer");
}

void AppDeferredRenderer::render(Maze* maze)
{
    // 0 geometry 1 point light 2 directional light
    const auto& geometryShader = shader[0];
    const auto& pointLightShader = shader[1];

    geometryShader.use();
    auto width = 0;
    auto height = 0;
    glfwGetFramebufferSize(App::getInstance()->getWindow(), &width, &height);

    // geometry pass
    gBuffer->bindForWriting();

    // only geometry pass writes to depth buffer and does depth tests
    glDepthMask(GL_TRUE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    for (auto* node : maze->getNodes())
    {
        node->draw(geometryShader);
    }

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    // setup for light pass
    // will blend multiple light passes
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);

    gBuffer->bindForReading();
    glClear(GL_COLOR_BUFFER_BIT);

    // point light pass
    pointLightShader.use();

    // camera uniforms
    const auto& c = maze->getCamera();
    pointLightShader.setMat4f("VP", c.getVP());
    pointLightShader.setVec3f("camWorldPos", c.getCamWorldPos());

    // need light position and distance
    for (const auto& l : maze->getActiveNode()->getPointLights())
    {
        // only one light per draw call so index is 0
        pointLightShader.setPointLight(l, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::string AppDeferredRenderer::getTypeName() const
{
    return getTypeNameStatic();
}