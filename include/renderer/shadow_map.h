#pragma once
#include <glad.h>

#include "renderer/app_shader.h"

class ShadowMap
{
public:
    // MUST BE EQUAL
    static const GLuint shadowWidth = 1024;
    static const GLuint shadowHeight = 1024;

    ShadowMap();
    ShadowMap(GLuint shadowWidth, GLuint shadowHeight);
    ~ShadowMap()
    {
        if (fbo != 0)
        {
            glDeleteFramebuffers(1, &fbo);
        }

        if (shadowMap != 0)
        {
            glDeleteTextures(1, &shadowMap);
        }
    }

    void setUniforms(AppShader& shader) const;
    void bindForWriting() const;
    void bindForReading(GLenum textureUnit) const;

    static ShadowMap* createShadowMap();
    static std::array<glm::mat4, 6> getShadowTransformations(const PointLight& l);

private:
    GLuint fbo;
    GLuint shadowMap;
};
