#pragma once
#include <glad/glad.h>
#include <renderer/app_shader.h>

class AppGBuffer
{
public:
    enum GBUFFER_TEXTURE_TYPE
    {
        GBUFFER_TEXTURE_TYPE_POSITION = 0,
        GBUFFER_TEXTURE_TYPE_DIFFUSE,
        GBUFFER_TEXTURE_TYPE_SPECULAR,
        GBUFFER_TEXTURE_TYPE_NORMAL,
        GBUFFER_NUM_TEXTURES
    };

    AppGBuffer() = delete;
    AppGBuffer(const AppGBuffer& appGBuffer) = delete;
    AppGBuffer(GLuint windowWidth, GLuint windowHeight);
    ~AppGBuffer()
    {
        if (fbo != 0)
        {
            glDeleteFramebuffers(1, &fbo);
        }

        if (textures[0] != 0)
        {
            glDeleteTextures(sizeof(textures) / sizeof(GBUFFER_TEXTURE_TYPE), textures);
        }

        if (depthTexture != 0)
        {
            glDeleteTextures(1, &depthTexture);
        }

        if (finalTexture != 0)
        {
            glDeleteTextures(1, &finalTexture);
        }
    };

    void startFrame() const;
    void bindForGeometryPass() const;
    void bindForStencilPass() const;
    void bindForLightPass() const;
    void bindForPortalPass(const AppShader& shader) const;
    void bindForFinalPass() const;
    void bindForFinalPortalPass() const;

    void setUniforms(const AppShader& shader) const;

private:
    GLuint fbo;
    GLuint textures[GBUFFER_NUM_TEXTURES];
    GLuint depthTexture;
    GLuint finalTexture;
};
