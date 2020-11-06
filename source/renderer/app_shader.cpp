#include "renderer/app_shader.h"
#include "global/config.h"

#include <iostream>
#include <fstream>
#include <sstream>

AppShader::AppShader(const char* name)
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
            std::cerr << "Error: No fragment shader found for shader " << fragmentShaderPath
                      << std::endl;
        }
        else
        {
            fragmentShaderId = compile(fragmentShaderPath, GL_FRAGMENT_SHADER);
            glAttachShader(programId, fragmentShaderId);
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
            glGetProgramInfoLog(programId, infoLogLength, NULL, &programErrorMessage[0]);
            std::cerr << "Error linking program: " << &programErrorMessage[0] << std::endl;
        }

        glDetachShader(programId, vertexShaderId);
        glDetachShader(programId, fragmentShaderId);

        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);
    }
    else
    {
        std::cerr << "Error creating shader " << name << ". Vertex shader file not found"
                  << std::endl;
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
        std::cerr << "Error compiling shader: Cannot open shader file " << path << std::endl;
        return GL_FALSE;
    }

    // compile
    char const* vertexSourcePointer = shaderCode.c_str();
    glShaderSource(id, 1, &vertexSourcePointer, NULL);
    glCompileShader(id);

    // check for errors
    GLint result = GL_FALSE;
    int infoLogLength;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength > 0)
    {
        std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(id, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
        std::cerr << "Error compiling shader " << path << ": " << &vertexShaderErrorMessage[0]
                  << std::endl;
    }

    std::cout << "Compiled shader " << path << std::endl;
    return id;
}

GLuint AppShader::getProgramId() const
{
    return programId;
}
