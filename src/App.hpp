#pragma once

#include "Dijkstra.hpp"
#include "Graph.hpp"
#include "Renderer.hpp"

#include <SFML/Graphics.hpp>

namespace dv
{

class App
{
public:
    // The graph must contain at least one node.
    explicit App(Graph graph);

    int run();

private:
    void handleEvent(const sf::Event& event);
    void recompute(bool restartAnimation = false);
    void startAnimation(bool play);
    void stopAnimation();
    void setStep(int step);
    void advanceAnimation(float seconds);
    void fitView();
    void updateViews();
    void zoomAt(sf::Vector2i pixel, float factor);
    sf::Vector2f toWorld(sf::Vector2i pixel) const;
    void updateHover(sf::Vector2i pixel);

    Graph graph_;
    ShortestPaths paths_;
    int start_ = 0;
    int end_ = 0;
    int pendingLink_ = -1;
    int hovered_ = -1;
    bool showHelp_ = true;

    // Step-by-step mode
    Trace trace_;
    Snapshot snapshot_;
    bool animating_ = false;
    bool playing_ = false;
    int step_ = 0;
    float stepTimer_ = 0.f;
    float stepsPerSecond_ = 1.5f;

    sf::Clock clock_;
    sf::RenderWindow window_;
    Renderer renderer_;
    sf::View worldView_;
    sf::View screenView_;
    sf::Vector2f center_;
    float zoom_ = 1.f; // world units per pixel

    bool panning_ = false;
    sf::Vector2i lastPixel_;
};

} // namespace dv
