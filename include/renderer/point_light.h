#pragma once

#include <glm/glm.hpp>
#include <vector>

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

    PointLight(
        const glm::vec3& pos,
        const glm::vec3& intensity,
        float dist,
        bool hasShadows,
        const std::vector<glm::vec4>& keyframes);

    [[nodiscard]] glm::vec3 getPos() const;
    [[nodiscard]] glm::vec3 getIntensity() const;
    [[nodiscard]] float getConstAttenuation() const;
    [[nodiscard]] float getLinAttenuation() const;
    [[nodiscard]] float getQuadAttenuation() const;
    [[nodiscard]] float getDist() const;
    [[nodiscard]] bool hasShadows() const;
    [[nodiscard]] bool getIsDynamic() const;

    void update();

    void enableShadows();
    void disableShadows();
    void addKeyFrame(const glm::vec4& kf);
    void addKeyFrame(const glm::vec3& pos, float timeDeltaMilliseconds);

private:
    glm::vec3 pos;
    glm::vec3 intensity;
    float dist;
    float constAttenuation;
    float linAttenuation;
    float quadAttenuation;
    bool shadows = false;
    bool isDynamic = false;
    // pos.xyz + timeDelta
    std::vector<glm::vec4> keyframes;
    int lastKeyFrameIndex = -1;
    float lastTime = -1;
    float timeLeft = -1;

    static glm::vec3 attenuationFromDist(float dist);
};
