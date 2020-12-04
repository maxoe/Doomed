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
    ~AppDeferredRenderer()
    {
        delete boundingSphere;
    }

    void render(Maze* maze) override;

    [[nodiscard]] std::string getTypeName() const override;

    static std::string getTypeNameStatic()
    {
        return "deferred";
    }

private:
    std::array<AppShader, 3> shader{"deferred_geometry", "deferred_point_light"};
    std::shared_ptr<AppGBuffer> gBuffer;
    Model* boundingSphere;
};