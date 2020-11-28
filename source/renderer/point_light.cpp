#include "renderer/point_light.h"

PointLight::PointLight()
    : PointLight(glm::vec3(0.0f), glm::vec3(0.0f))
{
}

PointLight::PointLight(
    const glm::vec3& pos,
    const glm::vec3& intensity,
    float constAtt,
    float linAtt,
    float quadAtt)
    : pos(pos)
    , intensity(intensity)
    , constAttenuation(constAtt)
    , linAttenuation(linAtt)
    , quadAttenuation(quadAtt)
{
}

/* See http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation */
PointLight::PointLight(const glm::vec3& pos, const glm::vec3& intensity, float dist)
    : pos(pos)
    , intensity(intensity)
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