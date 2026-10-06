#include "Dijkstra.hpp"

#include <algorithm>
#include <functional>
#include <queue>
#include <utility>

namespace dv
{

std::vector<int> ShortestPaths::pathTo(int target) const
{
    std::vector<int> path;
    if (!reachable(target))
    {
        return path;
    }
    for (int node = target; node != -1; node = parent[node])
    {
        path.push_back(node);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

ShortestPaths dijkstra(const Graph& graph, int start)
{
    ShortestPaths result;
    result.start = start;
    result.distance.assign(graph.nodeCount(), INF);
    result.parent.assign(graph.nodeCount(), -1);

    if (!graph.isValid(start))
    {
        return result;
    }

    using Item = std::pair<int, int>; // (distance, node)
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> queue;

    result.distance[start] = 0;
    queue.push({0, start});

    while (!queue.empty())
    {
        const auto [dist, node] = queue.top();
        queue.pop();

        if (dist > result.distance[node])
        {
            continue; // stale queue entry
        }

        for (const Edge& edge : graph.neighbours(node))
        {
            const int candidate = dist + edge.weight;
            if (candidate < result.distance[edge.to])
            {
                result.distance[edge.to] = candidate;
                result.parent[edge.to] = node;
                queue.push({candidate, edge.to});
            }
        }
    }

    return result;
}

} // namespace dv
