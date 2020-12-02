#pragma once
#include "renderer/app_shader.h"
#include "renderer/app_g_buffer.h"

class AppDeferredShader
{
private:
    AppShader shader;
    AppGBuffer gBuffer;
};
