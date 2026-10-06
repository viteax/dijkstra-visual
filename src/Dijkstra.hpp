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

// One iteration of the algorithm: `node` is settled and its edges are relaxed.
struct Relaxation
{
    int to;
    int distance; // the new, shorter distance of `to`
};

struct Step
{
    int node;
    std::vector<Relaxation> relaxed;
};

using Trace = std::vector<Step>;

// State of the algorithm after a number of steps.
struct Snapshot
{
    std::vector<int> distance;
    std::vector<int> parent;
    std::vector<bool> settled;
    int current = -1;             // node settled by the latest step
    std::vector<int> relaxedNow;  // nodes improved by the latest step
};

// When `trace` is given, every step of the run is recorded into it.
ShortestPaths dijkstra(const Graph& graph, int start, Trace* trace = nullptr);

// Replays the first `steps` steps of `trace` (0 = nothing done yet).
Snapshot snapshotAt(int nodeCount, int start, const Trace& trace, int steps);

} // namespace dv
