#include "world/maze.h"

void Maze::draw(glm::vec3 ambient, glm::vec3 lightWorldPos, glm::vec3 lightIntensity)
{
    for (auto* node : nodes)
    {
        node->draw(ambient, lightWorldPos, lightIntensity);
    }
}

MazeNode* Maze::addNode()
{
    nodes.emplace_back(new MazeNode());
    return nodes.back();
}
