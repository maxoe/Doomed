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
    // void renderRecursive(const glm::vec3& toPortal, const std::vector<std::size_t>&
    // renderingOrder);
    void render();
    void update();

    MazeNode* addNode();
    void setActiveNode(std::size_t nodeIndex);
    void setActiveNode(const MazeNode* node);

    [[nodiscard]] const std::vector<MazeNode*>& getNodes();
    [[nodiscard]] MazeNode* getActiveNode() const;
    [[nodiscard]] AppCamera* getCamera();
    [[nodiscard]] std::size_t getNodeIndex(const MazeNode* node) const;

    [[nodiscard]] std::vector<std::size_t>
    getRenderingOrder(std::size_t depth = 0, bool allowCycles = false) const;

private:
    AppCamera camera;
    std::vector<MazeNode*> nodes;
    std::shared_ptr<AppRenderer> renderer;
    std::size_t activeIndex = 0;
};
