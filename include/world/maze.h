#pragma once

#include <vector>

#include "world/maze_node.h"
#include "renderer/app_renderer.h"

class Maze
{
public:
    Maze() = default;
    Maze(const Maze&) = delete;
    Maze(const std::string& rendererType);

    ~Maze()
    {
        for (auto* node : nodes)
        {
            delete node;
        }
    }

    void initialize();
    void render();
    void update();

    MazeNode* addNode();
    void setActiveNode(std::size_t nodeIndex);
    void setActiveNode(const MazeNode* node);

    [[nodiscard]] const std::vector<MazeNode*>& getNodes();
    [[nodiscard]] MazeNode* getActiveNode();
    [[nodiscard]] AppCamera* getCamera();
    [[nodiscard]] std::size_t getNodeIndex(const MazeNode* node) const;

private:
    AppCamera camera;
    std::vector<MazeNode*> nodes;
    std::shared_ptr<AppRenderer> renderer;
    std::size_t activeIndex = 0;
};
