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
    std::size_t num = 0;
    numShadowMaps.push_back(num);

    if (this->shadows)
    {
        for (auto* node : maze->getNodes())
        {
            for (auto& l : node->getPointLights())
            {
                if (l.hasShadows())
                {
                    shadowMaps.emplace_back(ShadowMap::createShadowMap(), &l);
                }
                else
                {
                    shadowMaps.emplace_back(ShadowMap::createDummy(), &l);
                }

                ++num;
            }

            numShadowMaps.push_back(num);
        }

        createShadowMaps(true);
    }
}

void AppDeferredRenderer::afterActiveNodeChange()
{
    for (auto& m : shadowMaps)
    {
        delete m.first;
    }

    shadowMaps.clear();

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
    shadowMapShader.setFloat("far_plane", 100.0f);

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

void AppDeferredRenderer::render(Portal* portal /*, const glm::vec3& toPortal*/)
{
    createShadowMaps();

    // const auto& renderingOrder = maze->getRenderingOrder(4, false);

    // for (auto& n : renderingOrder)
    //{
    // std::cout << n << " ";
    //}
    // std::cout << std::endl;

    // 0 geometry 1 point light 2 directional light 3 null pass
    auto& geometryShader = shader[0];
    auto& pointLightShader = shader[1];
    auto& dirLightShader = shader[2];
    auto& stencilPassShader = shader[3];
    auto& depthStencilPassShader = shader[4];
    auto& geometryPortalShader = shader[5];

    auto width = App::getInstance()->getWidth();
    auto height = App::getInstance()->getHeight();

    gBuffer->startFrame();

    // geometry pass
    gBuffer->bindForGeometryPass();
    geometryShader.use();

    // only geometry pass writes to depth buffer and does depth tests
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 vp;
    glm::vec3 camPos;
    MazeNode* nodeToDraw;

    if (portal == nullptr)
    {
        const auto* c = maze->getCamera();
        vp = c->getVP();
        camPos = c->getCamWorldPos();
        nodeToDraw = maze->getActiveNode();
    }
    else
    {
        const auto* c = maze->getCamera();
        vp = portal->getVirtualVPMatrix(*c /*, toPortal*/);
        camPos = portal->getVirtualCameraPosition(*c /*, toPortal*/);

        nodeToDraw = maze->getNodes().at(portal->getDestinationNode());

        // clip away stuff between portal and camera
        /*glm::vec4 portalPlane(
            portal->getVirtualCameraDirection(*c),
            -glm::dot(portal->getVirtualCameraDirection(*c), portal->getTargetPosition()));*/

        glm::vec4 portalPlane(
            portal->getTargetDirection(),
            -(glm::dot(portal->getTargetDirection(), portal->getTargetPosition())));

        geometryPortalShader.setVec4f("nearClipPortalPlane", portalPlane);

        glEnable(GL_CLIP_DISTANCE0);
    }

    geometryShader.setMat4f("VP", vp);
    nodeToDraw->draw(geometryShader);

    glDisable(GL_CLIP_DISTANCE0);

    // need depth buffer read only for stencil pass
    glDepthMask(GL_FALSE);

    glEnable(GL_STENCIL_TEST);

    // stencil pass setup
    stencilPassShader.use();

    // camera uniforms
    stencilPassShader.setMat4f("VP", vp);

    // TODO move this to gui or so
    bool ambient = true;

    for (auto i = numShadowMaps[maze->getNodeIndex(nodeToDraw)];
         i < numShadowMaps[maze->getNodeIndex(nodeToDraw) + 1];
         ++i)
    {
        auto& lightPair = shadowMaps[i];
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
        // one light so only one bit is significant
        glStencilMask(0x01);
        glStencilFunc(GL_ALWAYS, 0, ~0xff);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
        boundingSphere->draw(stencilPassShader);

        // point light pass
        pointLightShader.use();
        gBuffer->bindForLightPass();
        // glClear(GL_COLOR_BUFFER_BIT);
        glStencilFunc(GL_NOTEQUAL, 0, 0xff);
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
        pointLightShader.setMat4f("VP", vp);
        pointLightShader.setVec3f("camWorldPos", camPos);

        // TODO fix magic numbers
        pointLightShader.setFloat("far_plane", 100.0f);

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
    // const auto* n = maze->getActiveNode();
    if (nodeToDraw->getHasDirectionalLight() || ambient)
    {
        dirLightShader.use();
        gBuffer->setUniforms(dirLightShader);
        dirLightShader.setVec2f("screenSize", width, height);
        dirLightShader.setVec3f("camWorldPos", camPos);

        gBuffer->bindForLightPass();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
        glDisable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        if (ambient)
        {
            // TODO fix magic
            dirLightShader.setVec3f("ambient", glm::vec3(0.05f));
        }
        else
        {
            dirLightShader.setVec3f("ambient", glm::vec3(0.00f));
        }

        if (nodeToDraw->getHasDirectionalLight())
        {
            dirLightShader.setDirectionalLight(
                nodeToDraw->getDirectionalLightDirection(),
                nodeToDraw->getDirectionalLightIntensity());
        }
        else
        {
            dirLightShader.setDirectionalLight(glm::vec3(0.0f), glm::vec3(0.0f));
        }

        quad->setModelMatrix(glm::identity<glm::mat4>());
        quad->draw(dirLightShader);

        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }

    // draw portals
    for (auto* p : nodeToDraw->getPortals())
    {
        depthStencilPassShader.use();
        depthStencilPassShader.setMat4f("VP", vp);
        depthStencilPassShader.setVec2f(
            "portalResize",
            p->getSize() /
                glm::vec2(App::getInstance()->getWidth(), App::getInstance()->getHeight()));
        depthStencilPassShader.setVec2f("screenSize", width, height);

        gBuffer->bindForPortalPass(depthStencilPassShader);

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        p->draw(depthStencilPassShader);
    }

    if (portal == nullptr)
    {
        gBuffer->bindForFinalPass();
        glDepthMask(GL_TRUE);

        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        glBlitFramebuffer(
            0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }
    else
    {
        // glViewport(0, 0, App::getInstance()->getWidth(), App::getInstance()->getHeight());

        gBuffer->bindForFinalPortalPass();
        portal->bindForFinalPass();
        // glDepthMask(GL_TRUE);

        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }

    // reset
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::string AppDeferredRenderer::getTypeName() const
{
    return getTypeNameStatic();
}