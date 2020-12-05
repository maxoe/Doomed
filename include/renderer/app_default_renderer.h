#pragma once
#include <renderer/app_renderer.h>
#include "renderer/app_shader.h"

class Maze;

class AppDefaultRenderer : public AppRenderer
{
public:
    AppDefaultRenderer();

    void render(Maze* maze) override;

    [[nodiscard]] std::string getTypeName() const override;

    static std::string getTypeNameStatic()
    {
        return "default";
    }

private:
    AppShader shader;
};