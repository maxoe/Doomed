#pragma once

#include "renderer/mesh.h"

class TextureLoader
{
public:
    TextureLoader() = delete;
    static void load(std::vector<TextureData>& textures);

private:
};
