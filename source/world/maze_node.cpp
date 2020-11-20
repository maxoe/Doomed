#include "world/maze_node.h"

#include <glm/ext/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"

#include "renderer/model_loader.h"
#include "core/camera.h"
#include "core/logger.h"

MazeNode::MazeNode()
    : lightWorldPos(glm::vec3(0.0f, 0.0f, 0.0))
    , lightIntensity(glm::vec3(15.0f, 14.0f, 14.0f))
    , ambient(glm::vec3(0.1f, 0.1f, 0.1f))
{
}

void MazeNode::draw()
{
    shader.use();

    shader.setMat4f("VP", camera.getVP());
    shader.setVec3f("camWorldPos", camera.getCamWorldPos());

    shader.setVec3f("lightWorldPos", camera.getCamWorldPos());
    shader.setVec3f("lightIntensity", lightIntensity);
    shader.setVec3f("ambient", ambient);

    for (auto* model : models)
    {
        model->draw(shader);
    }
}

void MazeNode::addModel(std::string const& relModelPath, glm::mat4& modelMatrix)
{
    models.emplace_back(ModelLoader::load(relModelPath));
    models.back()->setModelMatrix(modelMatrix);
}

void MazeNode::addModel(std::string const& relModelPath)
{
    glm::mat4 modelMatrix(1.0f);
    addModel(relModelPath, modelMatrix);
}

Camera& MazeNode::getCamera()
{
    return camera;
}