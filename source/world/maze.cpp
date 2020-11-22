#include "world/maze.h"

void Maze::draw(glm::vec3 ambient)
{
    for (auto* node : nodes)
    {
        node->draw(ambient);
    }
}

MazeNode* Maze::addNode()
{
    nodes.emplace_back(new MazeNode());
    return nodes.back();
}
