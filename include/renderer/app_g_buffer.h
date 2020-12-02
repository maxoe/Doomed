#pragma once
#include <glad/glad.h>

class AppGBuffer
{
public:
    enum GBUFFER_TEXTURE_TYPE
    {
        GBUFFER_TEXTURE_TYPE_POSITION,
        GBUFFER_TEXTURE_TYPE_DIFFUSE,
        GBUFFER_TEXTURE_TYPE_NORMAL,
        GBUFFER_TEXTURE_TYPE_TEXCOORD,
        GBUFFER_NUM_TEXTURES
    };

    AppGBuffer() = default;
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

        if (depthRbo != 0)
        {
            glDeleteTextures(1, &depthRbo);
        }
    };

    void bindForWriting() const;
    void bindForReading() const;
    void setReadBuffer(GBUFFER_TEXTURE_TYPE textureType) const;

private:
    GLuint fbo;
    GLuint textures[GBUFFER_NUM_TEXTURES];
    GLuint depthRbo;
};
