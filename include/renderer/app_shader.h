#pragma once

#include "global/config.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <filesystem>

class AppShader
{
public:
    AppShader() = delete;
    AppShader(const AppShader&) = delete;
    AppShader(const char* name);
    GLuint compile(const std::filesystem::path& shaderPath, GLenum shaderType);
    GLuint getProgramId() const;
    void use() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3f(const std::string& name, float x, float y, float z) const;
    void setVec3f(const std::string& name, const glm::vec3& vec) const;
    void setVec4f(const std::string& name, float x, float y, float z, float w) const;
    void setVec4f(const std::string& name, const glm::vec4& vec) const;
    void setMat3f(const std::string& name, const glm::mat3& mat) const;
    void setMat4f(const std::string& name, const glm::mat4& mat) const;

private:
    inline static const auto shaderDir = std::filesystem::path(APP_SHADER_DIR);
    GLuint programId;
};
