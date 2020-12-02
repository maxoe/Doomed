#include "renderer/app_g_buffer.h"
#include "core/logger.h"

#include <glad/glad.h>

AppGBuffer::AppGBuffer(GLuint windowWidth, GLuint windowHeight)
    : fbo(0)
    , textures{0}
    , depthRbo(0)
{
    // fbo
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    // g-buffer textures
    glGenTextures(sizeof(textures) / sizeof(GBUFFER_TEXTURE_TYPE), textures);

    for (GLuint i = 0; i < sizeof(textures) / sizeof(GBUFFER_TEXTURE_TYPE); i++)
    {
        // create storage and attach texture as target
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
    }

    // depth
    glGenRenderbuffers(1, &depthRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRbo);

    // enable drawing to all textures
    GLuint drawBuffers[] = {
        GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(sizeof(drawBuffers) / sizeof(GLenum), drawBuffers);

    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_WORLD_ERROR("Error initializing framebuffer for g-buffer: " + std::to_string(status));
    }

    // restore default fbo
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void AppGBuffer::bindForWriting() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
}

void AppGBuffer::bindForReading() const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
}

void AppGBuffer::setReadBuffer(GBUFFER_TEXTURE_TYPE textureType) const
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + textureType);
}