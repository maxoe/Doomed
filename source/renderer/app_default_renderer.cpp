#include "../../build/include/renderer/app_default_renderer.h"

#include <iostream>

#include "core/app.h"
#include "core/logger.h"
#include "world/maze.h"
#include <renderer/model_loader.h>

#include <GLFW/glfw3.h>
AppDefaultRenderer::AppDefaultRenderer()
    : shader(getTypeNameStatic().c_str())
{
    LOG_RENDERER_INFO("Using " + getTypeNameStatic() + " renderer");
}

void AppDefaultRenderer::render(Maze* maze)

{
    shader.use();

    // camera uniforms
    const auto* c = maze->getCamera();
    shader.setMat4f("VP", c->getVP());
    shader.setVec3f("camWorldPos", c->getCamWorldPos());

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    for (auto* node : maze->getNodes())
    {
        node->setLightUniforms(shader);
        node->draw(shader);
    }
}

std::string AppDefaultRenderer::getTypeName() const
{
    return getTypeNameStatic();
}
