#include "GraphIO.hpp"

#include "Config.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

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

namespace
{

// Folder of the running executable, or an empty path if it can't be determined.
std::filesystem::path executableDirectory()
{
    namespace fs = std::filesystem;
    std::error_code error;

#if defined(_WIN32)
    wchar_t buffer[MAX_PATH];
    const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    if (length > 0 && length < MAX_PATH)
    {
        return fs::path(std::wstring(buffer, length)).parent_path();
    }
#elif defined(__APPLE__)
    char buffer[4096];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0)
    {
        return fs::weakly_canonical(buffer, error).parent_path();
    }
#else
    const fs::path path = fs::read_symlink("/proc/self/exe", error);
    if (!error)
    {
        return path.parent_path();
    }
#endif
    return {};
}

} // namespace

std::string findResource(const std::string& relativePath)
{
    namespace fs = std::filesystem;

    // Next to the executable first, so a downloaded release works from any folder.
    const fs::path nextToExecutable = executableDirectory() / relativePath;
    if (fs::exists(nextToExecutable))
    {
        return nextToExecutable.string();
    }
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
