#include "renderer/shadow_map.h"

#include "core/app.h"
#include "core/logger.h"

ShadowMap::ShadowMap(GLuint shadowWidth, GLuint shadowHeight)
    : fbo(0)
    , shadowMap(0)
{
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_DEPTH_COMPONENT,
        shadowWidth,
        shadowHeight,
        0,
        GL_DEPTH_COMPONENT,
        GL_FLOAT,
        nullptr);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

    // do not write to color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_WORLD_ERROR("Error initializing framebuffer for shadow map: " + std::to_string(status));
    }
}

void ShadowMap::setUniforms(AppShader& shader) const
{
    shader.setInt("shadowMap", 0);
}

void ShadowMap::bindForWriting() const
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
}

void ShadowMap::bindForReading(GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
}

ShadowMap* ShadowMap::createShadowMap()
{
    auto width = 0;
    auto height = 0;

    glfwGetFramebufferSize(App::getInstance()->getWindow(), &width, &height);

    return new ShadowMap(width, height);
}