#pragma once

#include <vector>

#include "world/maze_node.h"
#include "renderer/app_renderer.h"

class Maze
{
public:
    Maze() = default;
    Maze(const std::string& rendererType);

    ~Maze()
    {
        for (auto* node : nodes)
        {
            delete node;
        }
    }

    void draw();
    MazeNode* addNode();
    const std::vector<MazeNode*>& getNodes();

private:
    std::vector<MazeNode*> nodes;
    AppRenderer renderer;
};
