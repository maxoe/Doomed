#include "world/maze_node.h"

#include <glm/ext/matrix_transform.hpp>
#include "glm/gtx/string_cast.hpp"

#include "renderer/model_loader.h"
#include "core/camera.h"
#include "core/logger.h"

MazeNode::MazeNode()
{
}

void MazeNode::draw(glm::vec3 ambient, glm::vec3 lightWorldPos, glm::vec3 lightIntensity)
{
    shader.use();

    shader.setMat4f("VP", camera.getVP());
    shader.setVec3f("camWorldPos", camera.getCamWorldPos());

    shader.setVec3f("lightWorldPos", lightWorldPos);
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