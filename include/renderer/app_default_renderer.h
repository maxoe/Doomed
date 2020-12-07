#pragma once
#include <renderer/app_renderer.h>
#include "renderer/app_shader.h"
#include "renderer/shadow_map.h"

class Maze;

class AppDefaultRenderer : public AppRenderer
{
public:
    AppDefaultRenderer();
    AppDefaultRenderer(bool shadows);

    ~AppDefaultRenderer()
    {
        for (auto* m : shadowMaps)
        {
            delete m;
        }
    }

    void render(Maze* maze) override;

    [[nodiscard]] std::string getTypeName() const override;
    void createShadowMaps(Maze* maze) override;

    static std::string getTypeNameStatic()
    {
        return "default";
    }

private:
    AppShader shader;
    AppShader shadowMapShader = AppShader("shadow_map");
    std::vector<ShadowMap*> shadowMaps;
    bool shadows = false;
};