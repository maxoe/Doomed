#pragma once

#include <glm/glm.hpp>

class PointLight
{
public:
    PointLight();
    // PointLight(
    // const glm::vec3& pos,
    // const glm::vec3& intensity,
    // float constAtt,
    // float linAtt,
    // float quadAtt);

    PointLight(
        const glm::vec3& pos,
        const glm::vec3& intensity,
        float dist = 20.0f /*arbitrary*/,
        bool hasShadows = false,
        bool dynamic = false);

    [[nodiscard]] glm::vec3 getPos() const;
    [[nodiscard]] glm::vec3 getIntensity() const;
    [[nodiscard]] float getConstAttenuation() const;
    [[nodiscard]] float getLinAttenuation() const;
    [[nodiscard]] float getQuadAttenuation() const;
    [[nodiscard]] float getDist() const;
    [[nodiscard]] bool hasShadows() const;
    [[nodiscard]] bool getIsDynamic() const;

    void enableShadows();
    void disableShadows();

private:
    glm::vec3 pos;
    glm::vec3 intensity;
    float dist;
    float constAttenuation;
    float linAttenuation;
    float quadAttenuation;
    bool shadows = false;
    bool isDynamic = false;

    static glm::vec3 attenuationFromDist(float dist);
};
