#pragma once
#include <glad.h>

#include "renderer/app_shader.h"

class ShadowMap
{
public:
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

private:
    GLuint fbo;
    GLuint shadowMap;
};
