#include "renderer/app_shader.h"
#include "global/config.h"
#include "core/logger.h"

#include <fstream>
#include <sstream>
#include <vector>

#include <glm/gtc/type_ptr.hpp>

AppShader::AppShader()
    : AppShader("default")
{
    LOG_RENDERER_WARN("Falling back to default shader");
}

AppShader::AppShader(const char* name, bool geometryShader)
    : type(name)
{
    programId = glCreateProgram();

    // find by name
    std::filesystem::path vertexShaderPath(shaderDir / name);
    vertexShaderPath += APP_VERTEX_SHADER_EXTENSION;

    if (std::filesystem::exists(vertexShaderPath))
    {
        GLuint vertexShaderId = compile(vertexShaderPath, GL_VERTEX_SHADER);
        glAttachShader(programId, vertexShaderId);

        GLuint fragmentShaderId = GL_FALSE;
        std::filesystem::path fragmentShaderPath(shaderDir / name);
        fragmentShaderPath += APP_FRAMENT_SHADER_EXTENSION;
        if (!std::filesystem::exists(fragmentShaderPath))
        {
            LOG_RENDERER_ERROR(
                "Error: No fragment shader found for shader " + fragmentShaderPath.string());
        }
        else
        {
            fragmentShaderId = compile(fragmentShaderPath, GL_FRAGMENT_SHADER);
            glAttachShader(programId, fragmentShaderId);
        }

        GLuint geomShaderId = GL_FALSE;
        if (geometryShader)
        {
            std::filesystem::path geomShaderPath(shaderDir / name);
            geomShaderPath += APP_GEOMETRY_SHADER_EXTENSION;
            if (!std::filesystem::exists(geomShaderPath))
            {
                LOG_RENDERER_ERROR(
                    "Error: No geometry shader found for shader " + geomShaderPath.string());
            }
            else
            {
                geomShaderId = compile(geomShaderPath, GL_GEOMETRY_SHADER);
                glAttachShader(programId, geomShaderId);
            }
        }

        glLinkProgram(programId);

        // check for linking errors
        GLint result = GL_FALSE;
        int infoLogLength;
        glGetProgramiv(programId, GL_LINK_STATUS, &result);
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            std::vector<char> programErrorMessage(infoLogLength + 1);
            glGetProgramInfoLog(programId, infoLogLength, nullptr, &programErrorMessage[0]);
            LOG_RENDERER_ERROR("Error linking program: " + std::string(&programErrorMessage[0]));
        }

        glDetachShader(programId, vertexShaderId);
        glDetachShader(programId, fragmentShaderId);

        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);

        if (geometryShader)
        {
            glDetachShader(programId, geomShaderId);
            glDeleteShader(geomShaderId);
        }
    }
    else
    {
        LOG_RENDERER_ERROR(
            "Error creating shader " + std::string(name) + ". Vertex shader file not found");
    }
}

GLuint AppShader::compile(const std::filesystem::path& path, GLenum type)
{
    GLuint id = glCreateShader(type);

    // open
    std::string shaderCode;
    std::ifstream shaderStream(path, std::ios::in);

    if (shaderStream.is_open())
    {
        std::stringstream sstr;
        sstr << shaderStream.rdbuf();
        shaderCode = sstr.str();
        shaderStream.close();
    }
    else
    {
        LOG_RENDERER_ERROR("Error compiling shader: Cannot open shader file " + path.string());
        return GL_FALSE;
    }

    // compile
    char const* vertexSourcePointer = shaderCode.c_str();
    glShaderSource(id, 1, &vertexSourcePointer, nullptr);
    glCompileShader(id);

    // check for errors
    GLint result = GL_FALSE;
    int infoLogLength;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength > 0)
    {
        std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(id, infoLogLength, nullptr, &vertexShaderErrorMessage[0]);
        LOG_RENDERER_ERROR(
            "Error compiling shader " + path.string() + ": " + &vertexShaderErrorMessage[0]);
    }

    LOG_RENDERER_INFO("Compiled shader " + path.string());
    return id;
}

GLuint AppShader::getProgramId() const
{
    return programId;
}

void AppShader::use() const
{
    glUseProgram(programId);
}

void AppShader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(programId, name.c_str()), static_cast<int>(value));
}
void AppShader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
}
void AppShader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
}

void AppShader::setVec2f(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(programId, name.c_str()), x, y);
}

void AppShader::setVec2f(const std::string& name, const glm::vec2& vec) const
{
    glUniform2fv(glGetUniformLocation(programId, name.c_str()), 1, glm::value_ptr(vec));
}

void AppShader::setVec3f(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(programId, name.c_str()), x, y, z);
}

void AppShader::setVec3f(const std::string& name, const glm::vec3& vec) const
{
    glUniform3fv(glGetUniformLocation(programId, name.c_str()), 1, glm::value_ptr(vec));
}

void AppShader::setVec4f(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(programId, name.c_str()), x, y, z, w);
}

void AppShader::setVec4f(const std::string& name, const glm::vec4& vec) const
{
    glUniform4fv(glGetUniformLocation(programId, name.c_str()), 1, glm::value_ptr(vec));
}

void AppShader::setMat3f(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(
        glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void AppShader::setMat4f(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(
        glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void AppShader::setPointLight(const PointLight& pointLight, GLuint index) const
{
    if (index >= maxPointLights)
    {
        LOG_RENDERER_ERROR(
            "Error setting point light with index " + std::to_string(index) +
            " in shader: Index is larger than max point lights " + std::to_string(maxPointLights));
        return;
    }

    setVec3f(
        std::string("pointLights[") + std::to_string(index) + std::string("].pos"),
        pointLight.getPos());
    setVec3f(
        std::string("pointLights[") + std::to_string(index) + std::string("].intensity"),
        pointLight.getIntensity());
    setFloat(
        std::string("pointLights[") + std::to_string(index) + std::string("].constAtt"),
        pointLight.getConstAttenuation());
    setFloat(
        std::string("pointLights[") + std::to_string(index) + std::string("].linAtt"),
        pointLight.getLinAttenuation());
    setFloat(
        std::string("pointLights[") + std::to_string(index) + std::string("].quadAtt"),
        pointLight.getQuadAttenuation());
    setBool(
        std::string("pointLights[") + std::to_string(index) + std::string("].shadows"),
        pointLight.hasShadows());

    setInt("shadowMaps[" + std::to_string(index) + "]", index);
}

void AppShader::setDirectionalLight(const glm::vec3& dir, const glm::vec3& intensity) const
{
    setVec3f(std::string("directionalLightDir"), dir);
    setVec3f(std::string("directionalLightIntensity"), intensity);
    setBool(std::string("hasDirectionalLight"), true);
}

GLuint AppShader::getMaxPointLights() const
{
    return maxPointLights;
}

const std::string& AppShader::getType() const
{
    return type;
}

void AppShader::setMaxPointLights(GLuint num)
{
    maxPointLights = num;
}