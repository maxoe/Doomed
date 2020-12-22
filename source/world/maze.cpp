#include "world/maze.h"

#include <queue>

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

MazeNode* Maze::getActiveNode() const
{
    // TODO implement
    return nodes.empty() ? nullptr : nodes[activeIndex];
}

AppCamera* Maze::getCamera()
{
    return &camera;
}

std::size_t Maze::getNodeIndex(const MazeNode* node) const
{
    return std::distance(nodes.begin(), std::find(nodes.begin(), nodes.end(), node));
}

/*
 * Returns vector of node ids in the order of being scanned by a bfs, up to bfs distance depth
 */
std::vector<std::size_t> Maze::getRenderingOrder(std::size_t depth, bool allowCycles) const
{
    struct BFSAttributes
    {
        bool scanned = false;
        std::size_t dist = 0;
    };

    std::vector<BFSAttributes> attr(nodes.size());

    std::fill(attr.begin(), attr.end(), BFSAttributes{false});
    std::queue<std::size_t> q;
    std::vector<std::size_t> result;

    std::size_t start = getNodeIndex(getActiveNode());
    q.push(start);
    attr[start] = {true, 0};

    while (!q.empty())
    {
        const std::size_t nodeId = q.front();
        q.pop();

        result.emplace_back(nodeId);

        if (attr[nodeId].dist == depth)
        {
            continue;
        }

        for (auto neighborIt = nodes.at(nodeId)->getPortals().begin();
             neighborIt != nodes.at(nodeId)->getPortals().end();
             ++neighborIt)
        {
            std::size_t neighborId = neighborIt->getDestinationNode();
            auto& neighborAttr = attr[neighborId];

            if (!neighborAttr.scanned || allowCycles)
            {
                q.push(neighborId);
                neighborAttr = {true, attr[nodeId].dist + 1};
            }
        }
    }

    return result;
}
