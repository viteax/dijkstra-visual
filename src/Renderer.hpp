#pragma once

#include "Dijkstra.hpp"
#include "Graph.hpp"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

namespace dv
{

// Everything the renderer needs to know to draw one frame.
struct Scene
{
    const Graph& graph;
    const ShortestPaths& paths;
    int start = 0;
    int end = 0;
    int pendingLink = -1; // first node of a link that is being created
    int hovered = -1;
    bool showHelp = true;

    // Set while the algorithm is being animated.
    const Snapshot* snapshot = nullptr;
    int step = 0;
    int stepCount = 0;
    bool playing = false;
    float stepsPerSecond = 1.f;

    bool animating() const { return snapshot != nullptr; }
    bool finished() const { return snapshot != nullptr && step >= stepCount; }
};

class Renderer
{
public:
    explicit Renderer(sf::RenderTarget& target) : target_(target) {}

    bool loadFont(const std::string& path);

    // Draws the graph. The world view has to be active on the target.
    void drawWorld(const Scene& scene);

    // Draws the overlay. The screen-space view has to be active on the target.
    void drawHud(const Scene& scene);

private:
    void drawLine(sf::Vector2f from, sf::Vector2f to, float thickness, sf::Color color);
    void drawCircle(sf::Vector2f center, float radius, sf::Color fill, sf::Color outline, float outlineThickness);
    void drawLabel(const std::string& string, sf::Vector2f center, unsigned size, sf::Color color);
    void drawBadge(const std::string& string, sf::Vector2f center, sf::Color fill, sf::Color textColor);
    void drawNode(const Scene& scene, int node);

    sf::RenderTarget& target_;
    sf::Font font_;
    sf::Text text_;
};

} // namespace dv
