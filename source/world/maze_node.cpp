#include "world/maze_node.h"

#include <glm/ext/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"

#include "renderer/model_loader.h"
#include "renderer/point_light.h"
#include "core/camera.h"
#include "core/logger.h"

MazeNode::MazeNode()
    : ambient(glm::vec3(0.f))
    , directionalLightDir(0.f)
    , directionalLightIntensity(glm::vec3(0.f))
{
}

void MazeNode::draw()
{
    shader.use();

    shader.setMat4f("VP", camera.getVP());
    shader.setVec3f("camWorldPos", camera.getCamWorldPos());

    shader.setVec3f("ambient", ambient);

    if (hasDirectionalLight)
    {
        shader.setDirectionalLight(directionalLightDir, directionalLightIntensity);
    }

    for (std::size_t i = 0; i < pointLights.size(); ++i)
    {
        shader.setPointLight(pointLights[i], i);
    }

    shader.setInt(std::string("pointLightCount"), pointLights.size());

    for (auto* model : models)
    {
        model->draw(shader);
    }
}

MazeNode* MazeNode::addModel(const std::string& relModelPath, glm::mat4& modelMatrix)
{
    models.emplace_back(ModelLoader::load(relModelPath));
    models.back()->setModelMatrix(modelMatrix);

    return this;
}

MazeNode* MazeNode::addModel(const std::string& relModelPath)
{
    glm::mat4 modelMatrix(1.0f);
    addModel(relModelPath, modelMatrix);

    return this;
}

MazeNode* MazeNode::addPointLight(const glm::vec3& pos, const glm::vec3& intensity, float dist)
{
    if (pointLights.size() == shader.getMaxPointLights())
    {
        LOG_RENDERER_ERROR(
            "Error setting point light in maze node: Max point lights " +
            std::to_string(shader.getMaxPointLights()) + " exceeded.");
    }
    else
    {
        pointLights.emplace_back(PointLight(pos, intensity, dist));
    }

    return this;
}

Camera& MazeNode::getCamera()
{
    return camera;
}

glm::vec3 MazeNode::getDirectionalLightDirection() const
{
    return directionalLightDir;
}

[[nodiscard]] glm::vec3 MazeNode::getDirectionalLightIntensity() const
{
    return directionalLightIntensity;
}

MazeNode* MazeNode::setDirectionalLight(const glm::vec3& dir, const glm::vec3& intensity)
{
    directionalLightDir = dir;
    directionalLightIntensity = intensity;
    hasDirectionalLight = true;

    return this;
}