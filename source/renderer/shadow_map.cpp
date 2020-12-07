#include "renderer/shadow_map.h"

#include <array>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "core/app.h"
#include "core/logger.h"

ShadowMap::ShadowMap()
    : ShadowMap(shadowWidth, shadowHeight)
{
}

ShadowMap::ShadowMap(GLuint width, GLuint height)
    : fbo(0)
    , shadowMap(0)
{
    glGenFramebuffers(1, &fbo);

    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);

    for (GLuint i = 0; i < 6; ++i)
    {
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_DEPTH_COMPONENT,
            width,
            height,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            nullptr);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);

    // do not write to color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG_RENDERER_ERROR(
            "FRAMEBUFFER_INCOMPLETE while initializing framebuffer for shadow map: " +
            std::to_string(status));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
}

ShadowMap* ShadowMap::createShadowMap()
{
    return new ShadowMap();
}

std::array<glm::mat4, 6> ShadowMap::getShadowTransformations(const PointLight& l)
{
    const auto& lightPos = l.getPos();

    float near = 1.0f;
    float far = 25.0f;
    auto width = 0;
    auto height = 0;
    glfwGetFramebufferSize(App::getInstance()->getWindow(), &width, &height);

    glm::mat4 shadowProj =
        glm::perspective(glm::radians(90.0f), static_cast<float>(width) / height, near, far);

    return std::array<glm::mat4, 6>{
        shadowProj *
            glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
        shadowProj *
            glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
        shadowProj *
            glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
        shadowProj *
            glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
        shadowProj *
            glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
        shadowProj *
            glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))};
}