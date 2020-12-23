#pragma once
#include <renderer/app_renderer.h>
#include <renderer/model.h>

#include "renderer/app_g_buffer.h"
#include "renderer/app_shader.h"
#include "renderer/shadow_map.h"

#include <array>

class Maze;

class AppDeferredRenderer : public AppRenderer
{
public:
    AppDeferredRenderer();
    AppDeferredRenderer(bool shadows);

    AppDeferredRenderer(const AppDeferredRenderer& renderer) = delete;
    ~AppDeferredRenderer()
    {
        delete boundingSphere;
        delete quad;

        for (auto& m : shadowMaps)
        {
            delete m.first;
        }
    }

    void initialize(Maze* maze) override;
    void afterActiveNodeChange() override;
    void render(Portal* portal = nullptr) override;
    void createShadowMaps(bool updateAll = false) override;

    [[nodiscard]] std::string getTypeName() const override;

    static std::string getTypeNameStatic()
    {
        return "deferred";
    }

private:
    Maze* maze;

    std::array<AppShader, 6> shader{
        "deferred_geometry",
        "deferred_point_light",
        "deferred_dir_light",
        "null_pass",
        "null_pass_set_depth",
        "deferred_geometry_portal"};
    std::shared_ptr<AppGBuffer> gBuffer;
    Model* boundingSphere;
    Model* quad;

    bool shadows = false;
    AppShader shadowMapShader = AppShader("shadow_map", true);
    std::vector<std::pair<ShadowMap*, PointLight*>> shadowMaps;
};