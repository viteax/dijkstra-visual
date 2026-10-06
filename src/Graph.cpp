#include "Graph.hpp"

#include <cmath>

namespace dv
{

int Graph::addNode(sf::Vector2f position)
{
    positions_.push_back(position);
    adjacency_.emplace_back();
    return nodeCount() - 1;
}

bool Graph::addEdge(int a, int b)
{
    if (!isValid(a) || !isValid(b) || a == b || hasEdge(a, b))
    {
        return false;
    }

    const sf::Vector2f delta = positions_[a] - positions_[b];
    const int weight = static_cast<int>(std::lround(std::hypot(delta.x, delta.y)));

    adjacency_[a].push_back({b, weight});
    adjacency_[b].push_back({a, weight});
    links_.push_back({a, b, weight});
    return true;
}

bool Graph::hasEdge(int a, int b) const
{
    if (!isValid(a) || !isValid(b))
    {
        return false;
    }
    for (const Edge& edge : adjacency_[a])
    {
        if (edge.to == b)
        {
            return true;
        }
    }
    return false;
}

int Graph::nodeAt(sf::Vector2f point, float radius) const
{
    const float radiusSquared = radius * radius;
    for (int i = 0; i < nodeCount(); ++i)
    {
        const sf::Vector2f delta = positions_[i] - point;
        if (delta.x * delta.x + delta.y * delta.y <= radiusSquared)
        {
            return i;
        }
    }
    return -1;
}

} // namespace dv
