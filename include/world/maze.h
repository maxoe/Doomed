#pragma once

#include <vector>

#include "world/maze_node.h"

class Maze
{
public:
    Maze() = default;

    ~Maze()
    {
        for (auto* node : nodes)
        {
            delete node;
        }
    }

    void draw(glm::vec3 ambient);
    MazeNode* addNode();

private:
    std::vector<MazeNode*> nodes;
};
