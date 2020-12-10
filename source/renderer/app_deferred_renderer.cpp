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
    LOG_RENDERER_INFO("Using " + getTypeNameStatic() + " renderer");
}

AppDeferredRenderer::AppDeferredRenderer(bool renderShadows)
    : AppDeferredRenderer()
{
    shadows = renderShadows;
}

void AppDeferredRenderer::initialize(Maze* mazePtr)
{
    maze = mazePtr;

    auto width = 0;
    auto height = 0;

    glfwGetFramebufferSize(App::getInstance()->getWindow(), &width, &height);

    // init deferred shading
    gBuffer = std::make_shared<AppGBuffer>(width, height);
    boundingSphere = ModelLoader::load("deferred_shading/sphere.obj");
    quad = ModelLoader::load("deferred_shading/quad.obj");

    // init shadow mapping

    if (this->shadows)
    {
        for (auto& l : maze->getActiveNode()->getPointLights())
        {
            if (l.hasShadows())
            {
                shadowMaps.emplace_back(ShadowMap::createShadowMap(), &l);
            }
            else
            {
                shadowMaps.emplace_back(ShadowMap::createDummy(), &l);
            }
        }

        createShadowMaps(true);
    }
}

/*
 * Skips non-dynamic lights if updateAll is false
 */
void AppDeferredRenderer::createShadowMaps(bool updateAll)
{
    shadowMapShader.use();
    glViewport(0, 0, ShadowMap::shadowWidth, ShadowMap::shadowHeight);
    glEnable(GL_DEPTH_TEST);
    const auto* c = maze->getCamera();

    // TODO fix magic numbers
    shadowMapShader.setFloat("far_plane", 25.0f);

    for (auto& lightPair : shadowMaps)
    {
        if (lightPair.first->isDummy() || (!updateAll && !lightPair.second->getIsDynamic()))
        {
            continue;
        }

        lightPair.first->bindForWriting();
        glDepthMask(true);
        glClear(GL_DEPTH_BUFFER_BIT);

        const auto* l = lightPair.second;

        const auto& shadowTransformations = ShadowMap::getShadowTransformations(*l);

        for (GLuint i = 0; i < 6; ++i)
        {
            shadowMapShader.setMat4f(
                "shadowMatrices[" + std::to_string(i) + "]", shadowTransformations[i]);
        }

        shadowMapShader.setVec3f("lightPos", l->getPos());

        for (auto* node : maze->getNodes())
        {
            node->draw(shadowMapShader);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, App::getInstance()->getWidth(), App::getInstance()->getHeight());
}

void AppDeferredRenderer::render()
{
    createShadowMaps();

    // 0 geometry 1 point light 2 directional light 3 null pass
    auto& geometryShader = shader[0];
    auto& pointLightShader = shader[1];
    auto& dirLightShader = shader[2];
    auto& stencilPassShader = shader[3];

    auto width = App::getInstance()->getWidth();
    auto height = App::getInstance()->getHeight();

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

    const auto* c = maze->getCamera();
    geometryShader.setMat4f("VP", c->getVP());

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
    stencilPassShader.setMat4f("VP", c->getVP());

    // need light position and distance
    bool ambientIsDone = false;

    for (auto& lightPair : shadowMaps)
    {
        auto* l = lightPair.second;

        // setup bounding sphere
        float lightDist = l->getDist();
        glm::mat4 modelMatrix = glm::translate(glm::identity<glm::mat4>(), l->getPos());
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
        glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        // point light pass setup

        gBuffer->setUniforms(pointLightShader);
        pointLightShader.setVec2f("screenSize", width, height);

        // camera uniforms
        pointLightShader.setMat4f("VP", c->getVP());
        pointLightShader.setVec3f("camWorldPos", c->getCamWorldPos());

        // TODO fix magic numbers
        pointLightShader.setFloat("far_plane", 25.0f);

        pointLightShader.setVec3f("ambient", glm::vec3(0.00f));
        if (!ambientIsDone)
        {
            // smuggle in ambient
            pointLightShader.setVec3f("ambient", glm::vec3(0.05f));
            ambientIsDone = true;
        }

        // shadow mapping, unit GL_TEXTURE4 for shadow map, (0-3) are gbuffer textures
        lightPair.first->bindForReading(GL_TEXTURE4);

        // only one light per draw call
        pointLightShader.setPointLight(*l, 4, 0);

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
        dirLightShader.setVec2f("screenSize", width, height);
        dirLightShader.setDirectionalLight(
            n->getDirectionalLightDirection(), n->getDirectionalLightIntensity());
        // camera uniforms
        // dirLightShader.setMat4f("VP", c->getVP());
        dirLightShader.setVec3f("camWorldPos", c->getCamWorldPos());

        gBuffer->bindForLightPass();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
        glDisable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        if (!ambientIsDone)
        {
            // smuggle in ambient
            dirLightShader.setVec3f("ambient", glm::vec3(0.05f));
            ambientIsDone = true;
        }
        else
        {
            dirLightShader.setVec3f("ambient", glm::vec3(0.00f));
        }

        quad->setModelMatrix(glm::identity<glm::mat4>());
        quad->draw(dirLightShader);

        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
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