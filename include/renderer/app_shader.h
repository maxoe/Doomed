#pragma once

#include "global/config.h"
#include "renderer/point_light.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <filesystem>

class AppShader
{
public:
    AppShader();
    AppShader(const AppShader&) = delete;
    AppShader(const char* name, bool geometryShader = false);
    GLuint compile(const std::filesystem::path& shaderPath, GLenum shaderType);
    GLuint getProgramId() const;
    void use() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2f(const std::string& name, float x, float y) const;
    void setVec2f(const std::string& name, const glm::vec2& vec) const;
    void setVec3f(const std::string& name, float x, float y, float z) const;
    void setVec3f(const std::string& name, const glm::vec3& vec) const;
    void setVec4f(const std::string& name, float x, float y, float z, float w) const;
    void setVec4f(const std::string& name, const glm::vec4& vec) const;
    void setMat3f(const std::string& name, const glm::mat3& mat) const;
    void setMat4f(const std::string& name, const glm::mat4& mat) const;

    void setPointLight(const PointLight& pointLight, GLuint index) const;
    void
    setPointLight(const PointLight& pointLight, GLuint index, GLuint shadowMapIdOverride) const;
    void setDirectionalLight(const glm::vec3& dir, const glm::vec3& intensity) const;

    void setMaxPointLights(GLuint num);
    [[nodiscard]] GLuint getMaxPointLights() const;
    [[nodiscard]] const std::string& getType() const;

private:
    inline static const auto shaderDir = std::filesystem::path(APP_SHADER_DIR);
    const std::string type;
    GLuint programId;
    GLuint activeTexture = GL_TEXTURE0;

    // MUST FIT VALUE IN FRAGMENT SHADER
    GLuint maxPointLights = 10;
};
