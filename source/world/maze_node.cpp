#include "world/maze_node.h"

#include <glm/ext/matrix_transform.hpp>

#include "renderer/model_loader.h"
#include "core/camera.h"

MazeNode::MazeNode()
    : lightWorldPos(glm::vec3(3.0f, 7.0f, 3.0))
    , lightIntensity(glm::vec3(15.0f, 14.0f, 14.0f))
{
}

void MazeNode::draw()
{
    shader.use();

    shader.setMat4f("VP", camera.getVP());
    shader.setVec3f("camWorldPos", camera.getCamWorldPos());

    shader.setVec3f("lightWorldPos", lightWorldPos);
    shader.setVec3f("lightIntensity", lightIntensity);

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