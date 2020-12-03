#include "renderer/app_renderer.h"

#include <iostream>

#include "core/app.h"
#include "core/logger.h"
#include "world/maze.h"

#include <GLFW/glfw3.h>

// shader will be default initialized and fall back to "default"
AppRenderer::AppRenderer()
    : AppRenderer("default")
{
    LOG_RENDERER_WARN("Falling back to default renderer");
}

AppRenderer::AppRenderer(const std::string& renderType)
    : type(renderType)
    , shader(type.c_str())
    , gBuffer(nullptr)
{
    if (type == "deferred_geometry")
    {
        auto width = 0;
        auto height = 0;

        glfwGetFramebufferSize(App::getInstance()->getWindow(), &width, &height);

        gBuffer = new AppGBuffer(width, height);
    }
}

void AppRenderer::render(Maze* maze)
{
    if (type == "default")
    {
        renderDefault(maze);
    }
    else if (type == "deferred_geometry")
    {
        renderDeferred(maze);
    }
    else
    {
        LOG_RENDERER_ERROR("Unknown render type " + type);
    }
}

void AppRenderer::renderDefault(Maze* maze)
{
    shader.use();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto* node : maze->getNodes())
    {
        node->draw();
    }
}

void AppRenderer::renderDeferred(Maze* maze)
{
    shader.use();
    auto width = 0;
    auto height = 0;
    glfwGetFramebufferSize(App::getInstance()->getWindow(), &width, &height);
    // geometry pass
    gBuffer->bindForWriting();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto* node : maze->getNodes())
    {
        node->draw();
    }

    // light pass
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gBuffer->bindForReading();

    GLuint halfWidth = width / 2.0f;
    GLuint halfHeight = height / 2.0f;

    gBuffer->setReadBuffer(AppGBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
    glBlitFramebuffer(
        0, 0, width, height, 0, 0, halfWidth, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    gBuffer->setReadBuffer(AppGBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
    glBlitFramebuffer(
        0, 0, width, height, 0, halfHeight, halfWidth, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    gBuffer->setReadBuffer(AppGBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
    glBlitFramebuffer(
        0, 0, width, height, halfWidth, halfHeight, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    gBuffer->setReadBuffer(AppGBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD);
    glBlitFramebuffer(
        0, 0, width, height, halfWidth, 0, width, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const std::string& AppRenderer::getType() const
{
    return type;
}

const std::string& AppRenderer::getShaderType() const
{
    return shader.getType();
}

const AppShader* AppRenderer::getShader() const
{
    return &shader;
}