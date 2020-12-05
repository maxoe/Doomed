#pragma once
#include "renderer/app_shader.h"

class Maze;

class AppRenderer
{
public:
    virtual ~AppRenderer() = default;
    virtual void render(Maze* maze) = 0;
    [[nodiscard]] virtual std::string getTypeName() const = 0;
};