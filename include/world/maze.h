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

    void draw(glm::vec3 ambient, glm::vec3 lightWorldPos, glm::vec3 lightIntensity);
    MazeNode* addNode();

private:
    std::vector<MazeNode*> nodes;
};
