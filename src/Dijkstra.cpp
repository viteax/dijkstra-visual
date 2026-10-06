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

ShortestPaths dijkstra(const Graph& graph, int start, Trace* trace)
{
    ShortestPaths result;
    result.start = start;
    if (trace)
    {
        trace->clear();
    }
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

        Step step{node, {}};
        for (const Edge& edge : graph.neighbours(node))
        {
            const int candidate = dist + edge.weight;
            if (candidate < result.distance[edge.to])
            {
                result.distance[edge.to] = candidate;
                result.parent[edge.to] = node;
                queue.push({candidate, edge.to});
                step.relaxed.push_back({edge.to, candidate});
            }
        }
        if (trace)
        {
            trace->push_back(std::move(step));
        }
    }

    return result;
}

Snapshot snapshotAt(int nodeCount, int start, const Trace& trace, int steps)
{
    Snapshot snapshot;
    snapshot.distance.assign(nodeCount, INF);
    snapshot.parent.assign(nodeCount, -1);
    snapshot.settled.assign(nodeCount, false);

    if (start < 0 || start >= nodeCount)
    {
        return snapshot;
    }
    snapshot.distance[start] = 0;

    steps = std::min(steps, static_cast<int>(trace.size()));
    for (int i = 0; i < steps; ++i)
    {
        const Step& step = trace[i];
        snapshot.settled[step.node] = true;
        snapshot.current = step.node;
        snapshot.relaxedNow.clear();

        for (const Relaxation& relaxation : step.relaxed)
        {
            snapshot.distance[relaxation.to] = relaxation.distance;
            snapshot.parent[relaxation.to] = step.node;
            snapshot.relaxedNow.push_back(relaxation.to);
        }
    }
    return snapshot;
}

} // namespace dv
