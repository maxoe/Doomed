#include "renderer/app_g_buffer.h"
#include "core/logger.h"

#include <glad/glad.h>

AppGBuffer::AppGBuffer(GLuint windowWidth, GLuint windowHeight)
    : fbo(0)
    , textures{0}
    , depthTexture(0)
    , finalTexture(0)
{
    // fbo
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    // g-buffer textures
    glGenTextures(sizeof(textures) / sizeof(GBUFFER_TEXTURE_TYPE), textures);

    for (GLuint i = 0; i < sizeof(textures) / sizeof(GBUFFER_TEXTURE_TYPE); ++i)
    {
        // create storage and attach texture as target
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
    }

    // depth
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH24_STENCIL8,
        windowWidth,
        windowHeight,
        0,
        GL_DEPTH_STENCIL,
        GL_UNSIGNED_INT_24_8,
        nullptr);

    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    // final
    glGenTextures(1, &finalTexture);
    glBindTexture(GL_TEXTURE_2D, finalTexture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        windowWidth,
        windowHeight,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, finalTexture, 0);

    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_WORLD_ERROR("Error initializing framebuffer for g-buffer: " + std::to_string(status));
    }

    // restore default fbo
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void AppGBuffer::startFrame() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT4);
    glClear(GL_COLOR_BUFFER_BIT);
}

void AppGBuffer::bindForGeometryPass() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    // enable drawing to all textures
    GLenum drawBuffers[] = {
        GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(sizeof(drawBuffers) / sizeof(GLenum), drawBuffers);
}

void AppGBuffer::bindForStencilPass() const
{
    // zero pass
    glDrawBuffer(GL_NONE);
}

void AppGBuffer::bindForLightPass() const
{
    // draw into own color buffer because we need depth and stencil
    glDrawBuffer(GL_COLOR_ATTACHMENT4);

    for (GLuint i = 0; i < sizeof(textures) / sizeof(GBUFFER_TEXTURE_TYPE); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[GBUFFER_TEXTURE_TYPE_POSITION + i]);
    }
}

void AppGBuffer::bindForFinalPass() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT4);
}

void AppGBuffer::bindForFinalPortalPass() const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT4);
}

void AppGBuffer::bindForPortalPass(const AppShader& shader) const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT4);
}

void AppGBuffer::setUniforms(const AppShader& shader) const
{
    shader.setInt("positionMap", GBUFFER_TEXTURE_TYPE_POSITION);
    shader.setInt("diffuseMap", GBUFFER_TEXTURE_TYPE_DIFFUSE);
    shader.setInt("specularMap", GBUFFER_TEXTURE_TYPE_SPECULAR);
    shader.setInt("normalMap", GBUFFER_TEXTURE_TYPE_NORMAL);
}