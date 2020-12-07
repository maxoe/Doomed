#include "world/maze.h"

#include <renderer/app_deferred_renderer.h>
#include "renderer/app_default_renderer.h"

Maze::Maze(const std::string& rendererType)
{
    if (rendererType == "default")
    {
        renderer = std::make_shared<AppDefaultRenderer>(true);
    }
    else
    {
        renderer = std::make_shared<AppDeferredRenderer>();
    }
}

void Maze::draw()
{
    renderer->render(this);
}

MazeNode* Maze::addNode()
{
    nodes.emplace_back(new MazeNode());
    return nodes.back();
}

const std::vector<MazeNode*>& Maze::getNodes()
{
    return nodes;
}

MazeNode* Maze::getActiveNode()
{
    // TODO implement
    return nodes.empty() ? nullptr : nodes[0];
}

Camera* Maze::getCamera()
{
    return &camera;
}