#pragma once

#include "global/config.h"

#include <glad/glad.h>

#include <filesystem>

class AppShader
{
public:
    AppShader(const char* name);
    GLuint compile(const std::filesystem::path& shaderPath, GLenum shaderType);
    GLuint getProgramId() const;

private:
    inline static const auto shaderDir = std::filesystem::path(APP_SHADER_DIR);
    GLuint programId;
};
