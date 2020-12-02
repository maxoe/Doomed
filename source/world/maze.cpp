#include "world/maze.h"

#include "renderer/app_renderer.h"

Maze::Maze(const std::string& rendererType)
    : renderer(rendererType)
{
}

void Maze::draw()
{
    renderer.render(this);
}

MazeNode* Maze::addNode()
{
    nodes.emplace_back(new MazeNode(renderer.getShader()));
    return nodes.back();
}

const std::vector<MazeNode*>& Maze::getNodes()
{
    return nodes;
}