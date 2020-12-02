#pragma once
#include "renderer/app_g_buffer.h"
#include "renderer/app_shader.h"

class Maze;

class AppRenderer
{
public:
    AppRenderer();
    AppRenderer(const std::string& type);

    void render(Maze* maze);
    void renderDefault(Maze* maze);
    void renderDeferred(Maze* maze);

    const std::string& getType() const;
    const std::string& getShaderType() const;
    const AppShader* getShader() const;

private:
    std::string type;
    AppShader shader;
    AppGBuffer gBuffer;
};
