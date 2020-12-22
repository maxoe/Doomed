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

void AppDeferredRenderer::render()
{
    createShadowMaps();

    const auto& renderingOrder = maze->getRenderingOrder(4, false);

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
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // for (int i = 0; i < maze->getNodes().size(); ++i)
    //{
    // auto* node = maze->getNodes().at(i);

    // draw portals
    // resolve this hack
    // for (const auto& portal : node->getPortals())
    //{
    // if (node->getIsVisible())
    //{
    // place virtual camera
    /*geometryShader.setMat4f(
        "VP", maze->getNodes().at(0)->getPortals().at(0).getVirtualVPMatrix(*c));

    */

    const char portalStencilMask = 0xf0;
    const auto* c = maze->getCamera();

    // 1. draw geometry in scene and correct stencil values if portal occluded
    // 2. draw portals using corrected stencil buffer

    // overwrite value in stencil buffer if depth test succeeds
    // this draws the scene over the portal's area and corrects the stencil buffer for occluded
    // portals
    glDisable(GL_STENCIL_TEST);

    // draw scene normally
    geometryShader.setMat4f("VP", c->getVP());
    glStencilFunc(GL_EQUAL, ~portalStencilMask + 0, portalStencilMask);
    maze->getNodes().at(renderingOrder[0])->draw(geometryShader);

    // portals stencil pass
    gBuffer->bindForStencilPass();
    stencilPassShader.use();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_STENCIL_TEST);

    stencilPassShader.setMat4f("VP", c->getVP());

    // use highest 4 bits for portals but clear all
    glStencilMask(0xff);
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilMask(portalStencilMask);

    // stencil stuff
    // for (int i = maze->getNodes().size() - 1; i >= 0; --i)
    //{
    auto* node = maze->getNodes().at(renderingOrder[0]);

    // if (node->getIsVisible())
    //{

    // we set the stencil value to the node id and discard every fragment which would
    // overwrite an already processed fragment or is occluded
    // already processed fragments have a higher stencil value than the reference
    // TODO REMOVE TEMPORARY HACK
    glStencilFunc(GL_GEQUAL, ~portalStencilMask + (1 - 0), portalStencilMask);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    node->drawPortals(stencilPassShader);
    //}
    //}

    // clear depth in portal area
    gBuffer->bindForStencilPass();
    depthStencilPassShader.use();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_STENCIL_TEST);
    glStencilMask(GL_FALSE);

    depthStencilPassShader.setMat4f("VP", c->getVP());

    // reset depth where the portal is visible so it doesn't interfere with drawing the scene behind
    // the portal
    depthStencilPassShader.setFloat("depth", 1.0f);

    // for (int i = maze->getNodes().size() - 1; i >= 0; --i)
    //{
    node = maze->getNodes().at(renderingOrder[0]);

    // if (node->getIsVisible())
    //{

    // we set the stencil value to the node id and discard every fragment which would
    // overwrite an already processed fragment or is occluded
    // already processed fragments have a higher stencil value than the reference
    // TODO REMOVE TEMPORARY HACK
    glStencilFunc(GL_GEQUAL, ~portalStencilMask + (1 - 0), portalStencilMask);

    node->drawPortals(depthStencilPassShader);
    //}
    //}

    glDepthFunc(GL_LESS);

    // geometry pass for portals
    // discards fragments between camera and portal
    gBuffer->bindForGeometryPass();
    geometryPortalShader.use();

    // no depth test because information is in stencil buffer
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // draw scene behind portal with corrected stencil buffer
    // read only stencil buffer
    glStencilMask(0x0);
    glStencilFunc(GL_EQUAL, ~portalStencilMask + 1, portalStencilMask);

    // place virtual camera
    geometryPortalShader.setMat4f(
        "VP", maze->getNodes().at(renderingOrder[0])->getPortals().at(0).getVirtualVPMatrix(*c));

    // additional clip plane (portal plane) ax+bx+cx+d=0
    glm::vec4 portalPlane(
        maze->getNodes().at(renderingOrder[0])->getPortals().at(0).getTargetDirection(),
        -glm::dot(
            maze->getNodes().at(renderingOrder[0])->getPortals().at(0).getTargetDirection(),
            maze->getNodes().at(renderingOrder[0])->getPortals().at(0).getTargetPosition()));

    geometryPortalShader.setVec4f("nearClipPortalPlane", portalPlane);

    glEnable(GL_CLIP_DISTANCE0);
    maze->getNodes().at(renderingOrder[1])->draw(geometryPortalShader);
    glDisable(GL_CLIP_DISTANCE0);

    //}
    //}

    // draw scene
    /*geometryShader.setMat4f("VP", c->getVP());
    glStencilFunc(GL_EQUAL, ~portalStencilMask + 0, portalStencilMask);
    maze->getActiveNode()->draw(geometryShader);*/

    // END OF SHITTY HACK

    // need depth buffer read only for stencil pass
    glDepthMask(GL_FALSE);

    glEnable(GL_STENCIL_TEST);

    // stencil pass setup
    stencilPassShader.use();

    // camera uniforms
    stencilPassShader.setMat4f("VP", c->getVP());
    glStencilMask(~portalStencilMask);

    // TODO move this to gui or so
    bool ambient = true;

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
        // one light so only one bit is significant
        glStencilMask(0x01);
        glStencilFunc(GL_ALWAYS, 0, ~portalStencilMask);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
        boundingSphere->draw(stencilPassShader);

        // point light pass
        pointLightShader.use();
        gBuffer->bindForLightPass();
        glStencilFunc(GL_NOTEQUAL, 0, ~portalStencilMask);
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
    const auto* n = maze->getActiveNode();
    if (n->getHasDirectionalLight() || ambient)
    {
        dirLightShader.use();
        gBuffer->setUniforms(dirLightShader);
        dirLightShader.setVec2f("screenSize", width, height);
        dirLightShader.setVec3f("camWorldPos", c->getCamWorldPos());

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

        if (n->getHasDirectionalLight())
        {
            dirLightShader.setDirectionalLight(
                n->getDirectionalLightDirection(), n->getDirectionalLightIntensity());
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

    gBuffer->bindForFinalPass();
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // reset
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::string AppDeferredRenderer::getTypeName() const
{
    return getTypeNameStatic();
}