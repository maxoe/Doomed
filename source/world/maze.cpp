#include "world/maze.h"

void Maze::draw()
{
    for (auto* node : nodes)
    {
        node->draw();
    }
}

MazeNode* Maze::addNode()
{
    nodes.emplace_back(new MazeNode());
    return nodes.back();
}
