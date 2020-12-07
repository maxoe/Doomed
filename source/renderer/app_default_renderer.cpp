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

    shader.use();

    shadowMaps.back()->setUniforms(shadowMapShader);
    shadowMaps.back()->bindForReading(GL_TEXTURE0);

    // camera uniforms
    const auto* c = maze->getCamera();
    shadowMapShader.setMat4f("VP", c->getVP());
    shadowMapShader.setVec3f("camWorldPos", c->getCamWorldPos());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    for (auto* node : maze->getNodes())
    {
        node->setLightUniforms(shader);
        // texture unit 0 is shadow map
        node->draw(shader, 1);
    }
}

std::string AppDefaultRenderer::getTypeName() const
{
    return getTypeNameStatic();
}