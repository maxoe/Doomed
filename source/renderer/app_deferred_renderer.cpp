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
    // 0 geometry 1 point light 2 directional light 3 null pass
    const auto& geometryShader = shader[0];
    const auto& pointLightShader = shader[1];
    const auto& dirLightShader = shader[2];
    const auto& stencilPassShader = shader[3];

    auto width = 0;
    auto height = 0;
    glfwGetFramebufferSize(App::getInstance()->getWindow(), &width, &height);

    gBuffer->startFrame();

    // geometry pass
    geometryShader.use();
    gBuffer->bindForGeometryPass();

    // only geometry pass writes to depth buffer and does depth tests
    glDepthMask(GL_TRUE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    const auto& c = maze->getCamera();
    geometryShader.setMat4f("VP", c.getVP());

    for (auto* node : maze->getNodes())
    {
        node->draw(geometryShader);
    }

    // need depth buffer read only for stencil pass
    glDepthMask(GL_FALSE);

    glEnable(GL_STENCIL_TEST);

    // stencil pass setup
    stencilPassShader.use();

    // camera uniforms
    stencilPassShader.setMat4f("VP", c.getVP());

    // point light pass setup
    pointLightShader.use();
    gBuffer->setUniforms(pointLightShader);
    pointLightShader.setVec2f("screenSize", width, height);

    // camera uniforms
    pointLightShader.setMat4f("VP", c.getVP());
    pointLightShader.setVec3f("camWorldPos", c.getCamWorldPos());

    // need light position and distance
    for (const auto& l : maze->getActiveNode()->getPointLights())
    {
        // setup bounding sphere
        float lightDist = l.getDist();
        glm::mat4 modelMatrix = glm::translate(glm::identity<glm::mat4>(), l.getPos());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(lightDist));
        boundingSphere->setModelMatrix(modelMatrix);

        // stencil pass
        stencilPassShader.use();
        gBuffer->bindForStencilPass();
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glClear(GL_STENCIL_BUFFER_BIT);
        // succeed always but write stencil buffer
        glStencilFunc(GL_ALWAYS, 0, 0);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
        boundingSphere->draw(stencilPassShader);

        // point light pass
        pointLightShader.use();
        gBuffer->bindForLightPass();
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        // only one light per draw call so index is 0
        pointLightShader.setPointLight(l, 0);
        boundingSphere->draw(pointLightShader);

        glCullFace(GL_BACK);
        glDisable(GL_BLEND);
    }

    glDisable(GL_STENCIL_TEST);

    // directional light pass
    const auto* n = maze->getActiveNode();
    if (n->getHasDirectionalLight())
    {
        dirLightShader.use();
        gBuffer->setUniforms(dirLightShader);
        gBuffer->bindForLightPass();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        dirLightShader.setVec2f("screenSize", width, height);
        dirLightShader.setDirectionalLight(
            n->getDirectionalLightDirection(), n->getDirectionalLightIntensity());

        quad->draw(pointLightShader);

        glDisable(GL_BLEND);
    }

    gBuffer->bindForFinalPass();
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // reset
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::string AppDeferredRenderer::getTypeName() const
{
    return getTypeNameStatic();
}