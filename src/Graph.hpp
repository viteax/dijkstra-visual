#pragma once

#include <SFML/System/Vector2.hpp>

#include <vector>

namespace dv
{

struct Edge
{
    int to;
    int weight;
};

struct Link
{
    int a;
    int b;
    int weight;
};

// Undirected weighted graph whose nodes live on a 2D plane.
// An edge weight is the rounded Euclidean distance between its endpoints.
class Graph
{
public:
    int addNode(sf::Vector2f position);

    // Returns false for invalid indices, self-loops and already existing links.
    bool addEdge(int a, int b);

    bool hasEdge(int a, int b) const;
    bool isValid(int node) const { return node >= 0 && node < nodeCount(); }
    bool empty() const { return positions_.empty(); }

    int nodeCount() const { return static_cast<int>(positions_.size()); }
    int edgeCount() const { return static_cast<int>(links_.size()); }

    sf::Vector2f position(int node) const { return positions_[node]; }
    const std::vector<sf::Vector2f>& positions() const { return positions_; }
    const std::vector<Edge>& neighbours(int node) const { return adjacency_[node]; }
    const std::vector<Link>& links() const { return links_; }

    // Index of the first node closer than `radius` to `point`, or -1.
    int nodeAt(sf::Vector2f point, float radius) const;

private:
    std::vector<sf::Vector2f> positions_;
    std::vector<std::vector<Edge>> adjacency_;
    std::vector<Link> links_;
};

} // namespace dv
