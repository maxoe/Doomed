#include "world/maze.h"

#include "renderer/app_deferred_renderer.h"
#include "renderer/app_default_renderer.h"
#include "core/logger.h"

Maze::Maze(const std::string& rendererType)
{
    if (rendererType == "default")
    {
        renderer = std::make_shared<AppDefaultRenderer>(true);
    }
    else
    {
        renderer = std::make_shared<AppDeferredRenderer>(true);
    }
}

void Maze::initialize()
{
    renderer->initialize(this);

    if (nodes.empty())
    {
        LOG_WORLD_WARN("Initialising maze with zero nodes");
    }
    else
    {
        nodes.at(0)->setIsVisible(true);
    }
}

void Maze::update()
{
    for (auto* node : nodes)
    {
        if (node->getIsVisible())
        {
            node->update();
        }
    }
}

void Maze::render()
{
    if (!nodes.empty())
    {
        renderer->render();
    }
}

MazeNode* Maze::addNode()
{
    nodes.emplace_back(new MazeNode());
    return nodes.back();
}

void Maze::setActiveNode(std::size_t nodeIndex)
{
    nodes[activeIndex]->setIsVisible(false);
    activeIndex = nodeIndex;
    nodes[activeIndex]->setIsVisible(true);

    renderer->afterActiveNodeChange();
}

void Maze::setActiveNode(const MazeNode* node)
{
    activeIndex = getNodeIndex(node);
}

const std::vector<MazeNode*>& Maze::getNodes()
{
    return nodes;
}

MazeNode* Maze::getActiveNode()
{
    // TODO implement
    return nodes.empty() ? nullptr : nodes[activeIndex];
}

Camera* Maze::getCamera()
{
    return &camera;
}

std::size_t Maze::getNodeIndex(const MazeNode* node) const
{
    return std::distance(nodes.begin(), std::find(nodes.begin(), nodes.end(), node));
}
