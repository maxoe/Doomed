#include "../../build/include/renderer/app_default_renderer.h"

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

AppDefaultRenderer::AppDefaultRenderer(bool shadows)
    : AppDefaultRenderer()
{
    this->shadows = shadows;

    if (this->shadows)
    {
        shadowMaps.emplace_back(ShadowMap::createShadowMap());
    }
}

void AppDefaultRenderer::createShadowMaps(Maze* maze)
{
    shadowMapShader.use();

    shadowMaps.back()->bindForWriting();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    const auto* c = maze->getCamera();
    const auto& l = maze->getActiveNode()->getPointLights()[0];
    shadowMapShader.setMat4f(
        "VP",
        glm::perspective(glm::radians(90.0f), 3840.f / 2160.f, 0.01f, 100.0f) *
            glm::lookAt(l.getPos(), c->getCamWorldPos() - l.getPos(), glm::vec3(0.0f, 1.0f, 0.0f)));

    for (auto* node : maze->getNodes())
    {
        node->draw(shadowMapShader);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void AppDefaultRenderer::render(Maze* maze)

{
    if (shadows)
    {
        createShadowMaps(maze);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shadowMaps.back()->setUniforms(shadowMapShader);
    shadowMaps.back()->bindForReading(GL_TEXTURE0);

    // shadowMapShader.setInt(std::string("shadowMap"), 0);
    // shadowMaps.back().bindForReading(GL_TEXTURE0);

    /*glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
    glm::mat4 depthViewMatrix = glm::lookAt(
        maze->getActiveNode()->getPointLights()[0].getPos(),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0));
    glm::mat4 depthModelMatrix = glm::mat4(1.0);
    glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;*/
    auto quad = ModelLoader::load("deferred_shading/quad.obj");
    quad->setModelMatrix(glm::identity<glm::mat4>());

    // shader.use();
    // camera uniforms
    const auto* c = maze->getCamera();
    shadowMapShader.setMat4f("VP", glm::identity<glm::mat4>());
    shadowMapShader.setVec3f("camWorldPos", c->getCamWorldPos());
    quad->draw(shadowMapShader);

    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    // for (auto* node : maze->getNodes())
    //{
    //    node->setLightUniforms(shader);
    //    node->draw(shader);
    //}
}

std::string AppDefaultRenderer::getTypeName() const
{
    return getTypeNameStatic();
}