#include "GraphIO.hpp"

#include "Config.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

namespace dv
{

void generateRandom(Graph& graph, int count, std::mt19937& rng)
{
    constexpr int MAX_ATTEMPTS = 1000;
    const float margin = 2.f * NODE_RADIUS;

    std::uniform_real_distribution<float> randomX(margin, WORLD_WIDTH - margin);
    std::uniform_real_distribution<float> randomY(margin, WORLD_HEIGHT - margin);

    for (int i = 0; i < count; ++i)
    {
        // The field may simply be full; stop instead of looping forever.
        bool placed = false;
        for (int attempt = 0; attempt < MAX_ATTEMPTS && !placed; ++attempt)
        {
            const sf::Vector2f candidate(randomX(rng), randomY(rng));
            if (graph.nodeAt(candidate, NODE_SPACING) == -1)
            {
                graph.addNode(candidate);
                placed = true;
            }
        }
        if (!placed)
        {
            std::cerr << "Only " << graph.nodeCount() << " of " << count
                      << " points fit on the field.\n";
            break;
        }
    }

    // Every node is linked to one or two of its nearest later neighbours. Each node
    // has a link towards a higher index, so the graph stays connected.
    const int n = graph.nodeCount();
    for (int i = 0; i < n - 1; ++i)
    {
        std::vector<std::pair<float, int>> candidates;
        for (int j = i + 1; j < n; ++j)
        {
            const sf::Vector2f delta = graph.position(j) - graph.position(i);
            candidates.push_back({delta.x * delta.x + delta.y * delta.y, j});
        }
        const std::size_t keep = std::min<std::size_t>(candidates.size(), 3);
        std::partial_sort(candidates.begin(), candidates.begin() + keep, candidates.end());

        std::uniform_int_distribution<int> linkCount(1, 2);
        std::uniform_int_distribution<std::size_t> pick(0, keep - 1);
        graph.addEdge(i, candidates[0].second); // guarantees connectivity
        if (linkCount(rng) == 2)
        {
            graph.addEdge(i, candidates[pick(rng)].second);
        }
    }
}

bool loadNodes(const std::string& path, Graph& graph)
{
    std::ifstream file(path);
    if (!file)
    {
        return false;
    }
    float x, y;
    while (file >> x >> y)
    {
        graph.addNode({x, y});
    }
    return true;
}

bool loadLinks(const std::string& path, Graph& graph)
{
    std::ifstream file(path);
    if (!file)
    {
        return false;
    }
    int a, b;
    while (file >> a >> b)
    {
        if (!graph.addEdge(a, b))
        {
            std::cerr << path << ": skipped link " << a << " - " << b << '\n';
        }
    }
    return true;
}

std::string findResource(const std::string& relativePath)
{
    namespace fs = std::filesystem;

    if (fs::exists(relativePath))
    {
        return relativePath;
    }
#ifdef DIJKSTRA_RESOURCE_DIR
    const fs::path fromSource = fs::path(DIJKSTRA_RESOURCE_DIR) / relativePath;
    if (fs::exists(fromSource))
    {
        return fromSource.string();
    }
#endif
    return relativePath;
}

} // namespace dv
