#include "world/maze.h"

#include <iostream>
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
    bool needsRendererUpdate = false;

    for (auto* node : nodes)
    {
        if (node->getIsVisible())
        {
            needsRendererUpdate |= node->update();
        }
    }

    if (needsRendererUpdate)
    {
        renderer->afterActiveNodeChange();
    }
}

// void Maze::renderRecursive(
//    const glm::vec3& toPortal,
//    const std::vector<std::size_t>& renderingOrder)
//{
//    if (renderingOrder.empty())
//    {
//        return;
//    }
//
//    renderer->render(getNodes().at(renderingOrder.at(0))->getPortals().at(0), toPortal);
//
//    if (renderingOrder.size() == 1)
//    {
//        return;
//    }
//
//    glm::vec3 newToPortal =
//        toPortal + (getNodes().at(renderingOrder.at(1))->getPortals().at(0)->getCenterPoint() -
//                    getNodes().at(renderingOrder.at(0))->getPortals().at(0)->getTargetPosition());
//
//    renderRecursive(
//        newToPortal, std::vector<std::size_t>(renderingOrder.begin() + 1, renderingOrder.end()));
//}

void Maze::render()
{
    if (!nodes.empty())
    {
        const auto& order = getRenderingOrder(1);

        for (auto it = order.rbegin(); it != order.rend(); ++it)
        {
            for (auto* portal : getNodes().at(*it)->getPortals())
            {
                renderer->render(portal);
            }
        }

        /*const auto& renderingOrder = getRenderingOrder();

        renderRecursive(
            getNodes().at(renderingOrder.front())->getPortals().at(0)->getCenterPoint(),
            renderingOrder);*/

        renderer->render();
    }

    for (auto* node : getNodes())
    {
        for (auto* portal : node->getPortals())
        {
            // clears texture
            portal->bindForFinalPass();
        }
    }

    // renderer->render(getNodes().at(1)->getPortals().at(0));
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

        for (std::size_t i = 0; i < nodes.at(nodeId)->getPortals().size(); ++i)
        {
            std::size_t neighborId = nodes.at(nodeId)->getPortals().at(i)->getDestinationNode();
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
