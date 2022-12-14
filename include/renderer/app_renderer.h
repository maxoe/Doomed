#pragma once
#include "renderer/app_shader.h"

#include "world/portal.h"

class Maze;

class AppRenderer
{
public:
    virtual ~AppRenderer() = default;
    virtual void initialize(Maze* maze) = 0;
    virtual void afterActiveNodeChange() = 0;
    virtual void render(Portal* portal = nullptr) = 0;
    [[nodiscard]] virtual std::string getTypeName() const = 0;
    virtual void createShadowMaps(bool updateAll = false) = 0;
};