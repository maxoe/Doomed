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
    [[nodiscard]] const std::vector<MazeNode*>& getNodes();
    [[nodiscard]] MazeNode* getActiveNode();
    [[nodiscard]] Camera* getCamera();

private:
    Camera camera;
    std::vector<MazeNode*> nodes;
    std::shared_ptr<AppRenderer> renderer;
};
