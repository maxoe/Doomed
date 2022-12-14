#pragma once

#include <renderer/app_renderer.h>
#include "renderer/app_shader.h"
#include "renderer/shadow_map.h"

#include "world/portal.h"

class Maze;

class AppDefaultRenderer : public AppRenderer
{
public:
    AppDefaultRenderer();
    AppDefaultRenderer(bool shadows);

    AppDefaultRenderer(const AppDefaultRenderer& renderer) = delete;

    ~AppDefaultRenderer() override
    {
        for (auto& m : shadowMaps)
        {
            delete m.first;
        }
    }

    void initialize(Maze* maze) override;
    void afterActiveNodeChange() override;
    void render(Portal* portal = nullptr) override;

    [[nodiscard]] std::string getTypeName() const override;
    void createShadowMaps(bool updateAll = false) override;

    static std::string getTypeNameStatic()
    {
        return "default";
    }

private:
    Maze* maze;
    AppShader shader;

    bool shadows = false;
    AppShader shadowMapShader = AppShader("shadow_map", true);
    std::vector<std::pair<ShadowMap*, PointLight*>> shadowMaps;
};