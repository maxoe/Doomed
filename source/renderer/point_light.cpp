#include "renderer/point_light.h"

#include <GLFW/glfw3.h>

PointLight::PointLight()
    : PointLight(glm::vec3(0.0f), glm::vec3(0.0f))
{
}

// PointLight::PointLight(
//    const glm::vec3& pos,
//    const glm::vec3& intensity,
//    float constAtt,
//    float linAtt,
//    float quadAtt)
//    : pos(pos)
//    , intensity(intensity)
//    , constAttenuation(constAtt)
//    , linAttenuation(linAtt)
//    , quadAttenuation(quadAtt)
//{
//}

PointLight::PointLight(
    const glm::vec3& pos,
    const glm::vec3& intensity,
    float dist,
    bool hasShadows,
    const std::vector<glm::vec4>& initKeyframes)
    : PointLight(pos, intensity, dist, hasShadows, true)
{
    keyframes = initKeyframes;
}

/* See http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation */
PointLight::PointLight(
    const glm::vec3& pos,
    const glm::vec3& intensity,
    float dist,
    bool hasShadows,
    bool dynamic)
    : pos(pos)
    , intensity(intensity)
    , dist(dist)
    , shadows(hasShadows)
    , isDynamic(dynamic)
{
    const auto& att = attenuationFromDist(dist);
    constAttenuation = att[0];
    linAttenuation = att[1];
    quadAttenuation = att[2];
}

glm::vec3 PointLight::attenuationFromDist(float dist)
{
    float constant = 1.0f;

    float lin;
    float quad;

    if (dist > 600.0f)
    {
        lin = 0.0014f;
        quad = 0.000007f;
    }
    else if (dist > 325)
    {
        lin = 0.007f;
        quad = 0.0002f;
    }
    else if (dist > 200)
    {
        lin = 0.014f;
        quad = 0.0007f;
    }
    else if (dist > 160)
    {
        lin = 0.022f;
        quad = 0.0019f;
    }
    else if (dist > 100)
    {
        lin = 0.027f;
        quad = 0.0028f;
    }
    else if (dist > 65)
    {
        lin = 0.045f;
        quad = 0.0075f;
    }
    else if (dist > 50)
    {
        lin = 0.07f;
        quad = 0.017f;
    }
    else if (dist > 32)
    {
        lin = 0.09f;
        quad = 0.032f;
    }
    else if (dist > 20)
    {
        lin = 0.14f;
        quad = 0.07f;
    }
    else if (dist > 13)
    {
        lin = 0.22f;
        quad = 0.2f;
    }
    else if (dist > 7)
    {
        lin = 0.35f;
        quad = 0.44f;
    }
    else
    {
        lin = 0.7f;
        quad = 1.8f;
    }

    return glm::vec3(constant, lin, quad);
}

glm::vec3 PointLight::getPos() const
{
    return pos;
}

glm::vec3 PointLight::getIntensity() const
{
    return intensity;
}

float PointLight::getConstAttenuation() const
{
    return constAttenuation;
}

float PointLight::getLinAttenuation() const
{
    return linAttenuation;
}

float PointLight::getQuadAttenuation() const
{
    return quadAttenuation;
}

/*
 * Modified from http://ogldev.atspace.co.uk/www/tutorial36/tutorial36.html
 * It's larger than the dist which was used for initialization
 */
float PointLight::getDist() const
{
    /*float maxChannel = glm::max(glm::max(intensity.x, intensity.y), intensity.z);

    return (-linAttenuation + sqrtf(
                                  linAttenuation * linAttenuation -
                                  4 * quadAttenuation * (quadAttenuation - 256 * maxChannel))) /
           (2 * quadAttenuation);*/
    return dist;
}

bool PointLight::hasShadows() const
{
    return shadows;
}

void PointLight::update()
{
    if (!isDynamic)
    {
        return;
    }

    float time = glfwGetTime();
    float timeDelta = time - lastTime;
    timeLeft -= timeDelta;
    lastTime = time;
    int nextKeyFrameIndex = (lastKeyFrameIndex + 1) % keyframes.size();

    while (timeLeft < 0)
    {
        lastKeyFrameIndex = nextKeyFrameIndex;
        nextKeyFrameIndex = (nextKeyFrameIndex + 1) % keyframes.size();
        timeLeft = -timeLeft;
        timeLeft = keyframes.at(nextKeyFrameIndex).w - timeLeft;
    }

    // update position
    float relativeLeft = timeLeft / keyframes.at(nextKeyFrameIndex).w;
    pos = (1 - relativeLeft) * glm::vec3(keyframes.at(nextKeyFrameIndex)) +
          relativeLeft * glm::vec3(keyframes.at(lastKeyFrameIndex));

    // add more as necessary
}

void PointLight::addKeyFrame(const glm::vec4& kf)
{
    keyframes.emplace_back(kf);
}

void PointLight::addKeyFrame(const glm::vec3& keyFramePos, float timeDelta)
{
    addKeyFrame(glm::vec4(keyFramePos.x, keyFramePos.y, keyFramePos.z, timeDelta));
}

void PointLight::enableShadows()
{
    if (!isDynamic)
    {
        return;
    }

    shadows = true;
}

void PointLight::disableShadows()
{
    if (!isDynamic)
    {
        return;
    }

    shadows = false;
}

bool PointLight::getIsDynamic() const
{
    return isDynamic;
}
