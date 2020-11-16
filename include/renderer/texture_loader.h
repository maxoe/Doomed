#pragma once

#include <glad/glad.h>
#include "renderer/mesh.h"

class TextureLoader
{
public:
    TextureLoader() = delete;
    static GLuint load(const TextureData& td);

private:
};
