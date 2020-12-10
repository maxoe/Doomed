#include "../../build/include/renderer/app_default_renderer.h"

#include <array>
#include <iostream>

#include "core/app.h"
#include "core/logger.h"
#include "world/maze.h"
#include <renderer/model_loader.h>

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

AppDefaultRenderer::AppDefaultRenderer()
    : shader(getTypeNameStatic().c_str())
{
    LOG_RENDERER_INFO("Using " + getTypeNameStatic() + " renderer");
}

AppDefaultRenderer::AppDefaultRenderer(bool renderShadows)
    : AppDefaultRenderer()
{
    shadows = renderShadows;
}

void AppDefaultRenderer::initialize(Maze* mazePtr)
{
    maze = mazePtr;

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
void AppDefaultRenderer::createShadowMaps(bool updateAll)
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

void AppDefaultRenderer::render()

{
    createShadowMaps();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();

    // camera uniforms
    const auto* c = maze->getCamera();
    shader.setMat4f("VP", c->getVP());
    shader.setVec3f("camWorldPos", c->getCamWorldPos());

    // TODO fix magic numbers
    shader.setFloat("far_plane", 25.0f);
    shader.setVec3f("ambient", glm::vec3(0.05f));

    shader.setInt("pointLightCount", shadowMaps.size());

    if (maze->getActiveNode()->getHasDirectionalLight())
    {
        shader.setDirectionalLight(
            maze->getActiveNode()->getDirectionalLightDirection(),
            maze->getActiveNode()->getDirectionalLightIntensity());
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    GLuint lightNr = 0;
    for (auto& lightPair : shadowMaps)
    {
        bool withShadows = !lightPair.first->isDummy();

        if (withShadows)
        {
            lightPair.first->bindForReading(GL_TEXTURE0 + lightNr);
        }

        shader.setPointLight(*lightPair.second, lightNr++);
    }

    for (GLuint i = lightNr; i < shader.getMaxPointLights(); ++i)
    {
        shader.setInt("shadowMaps[" + std::to_string(i) + "]", 0);
    }

    for (auto* node : maze->getNodes())
    {
        // texture unit 0 is shadow map
        node->draw(shader, lightNr);
    }
}

std::string AppDefaultRenderer::getTypeName() const
{
    return getTypeNameStatic();
}