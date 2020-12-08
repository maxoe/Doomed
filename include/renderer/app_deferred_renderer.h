#pragma once
#include <renderer/app_renderer.h>
#include <renderer/model.h>

#include "renderer/app_g_buffer.h"
#include "renderer/app_shader.h"

#include <array>

class Maze;

class AppDeferredRenderer : public AppRenderer
{
public:
    AppDeferredRenderer();
    AppDeferredRenderer(const AppDeferredRenderer& renderer) = delete;
    ~AppDeferredRenderer()
    {
        delete boundingSphere;
    }

    void initialize(Maze* maze) override;
    void render() override;
    void createShadowMaps(bool updateAll = false) override;

    [[nodiscard]] std::string getTypeName() const override;

    static std::string getTypeNameStatic()
    {
        return "deferred";
    }

private:
    Maze* maze;

    std::array<AppShader, 4> shader{
        "deferred_geometry",
        "deferred_point_light",
        "deferred_dir_light",
        "null_pass"};
    std::shared_ptr<AppGBuffer> gBuffer;
    Model* boundingSphere;
    Model* quad;
};