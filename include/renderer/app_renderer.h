#pragma once
#include "renderer/app_shader.h"

class Maze;

class AppRenderer
{
public:
    virtual ~AppRenderer() = default;
    virtual void initialize(Maze* maze) = 0;
    virtual void render() = 0;
    [[nodiscard]] virtual std::string getTypeName() const = 0;
    virtual void createShadowMaps(bool updateAll = false) = 0;
};