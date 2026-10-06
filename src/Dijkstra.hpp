#pragma once

#include "Graph.hpp"

#include <limits>
#include <vector>

namespace dv
{

constexpr int INF = std::numeric_limits<int>::max();

struct ShortestPaths
{
    int start = -1;
    std::vector<int> distance; // INF when a node can't be reached
    std::vector<int> parent;   // previous node on the shortest path, -1 if none

    bool reachable(int node) const
    {
        return node >= 0 && node < static_cast<int>(distance.size()) && distance[node] != INF;
    }

    // Nodes from `start` to `target` inclusive; empty if `target` is unreachable.
    std::vector<int> pathTo(int target) const;
};

ShortestPaths dijkstra(const Graph& graph, int start);

} // namespace dv
