#include "App.hpp"

#include "Config.hpp"
#include "GraphIO.hpp"
#include "Theme.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace dv
{

namespace
{

void openWindow(sf::RenderWindow& window)
{
    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const unsigned width = std::min(1600u, desktop.width * 9 / 10);
    const unsigned height = std::min(900u, desktop.height * 9 / 10);

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    window.create(sf::VideoMode(width, height), "Dijkstra's Algorithm Visualization",
                  sf::Style::Default, settings);
    window.setFramerateLimit(60);
}

} // namespace

App::App(Graph graph)
    : graph_(std::move(graph)),
      renderer_(window_)
{
    openWindow(window_);
    end_ = graph_.nodeCount() - 1;

    if (!renderer_.loadFont(findResource("assets/fonts/Roboto-Regular.ttf")))
    {
        std::cerr << "Can't load assets/fonts/Roboto-Regular.ttf\n";
    }

    recompute();
    fitView();
}

int App::run()
{
    while (window_.isOpen())
    {
        sf::Event event;
        while (window_.pollEvent(event))
        {
            handleEvent(event);
        }

        const Scene scene{graph_, paths_, start_, end_, pendingLink_, hovered_, showHelp_};

        window_.clear(theme::BACKGROUND);
        window_.setView(worldView_);
        renderer_.drawWorld(scene);
        window_.setView(screenView_);
        renderer_.drawHud(scene);
        window_.display();
    }
    return 0;
}

void App::handleEvent(const sf::Event& event)
{
    switch (event.type)
    {
    case sf::Event::Closed:
        window_.close();
        break;

    case sf::Event::Resized:
        updateViews();
        break;

    case sf::Event::MouseWheelScrolled:
        zoomAt({event.mouseWheelScroll.x, event.mouseWheelScroll.y}, std::pow(0.9f, event.mouseWheelScroll.delta));
        break;

    case sf::Event::MouseMoved:
    {
        const sf::Vector2i pixel(event.mouseMove.x, event.mouseMove.y);
        if (panning_)
        {
            center_ += toWorld(lastPixel_) - toWorld(pixel);
            updateViews();
        }
        lastPixel_ = pixel;
        updateHover(pixel);
        break;
    }

    case sf::Event::MouseButtonPressed:
    {
        const sf::Vector2i pixel(event.mouseButton.x, event.mouseButton.y);
        if (event.mouseButton.button == sf::Mouse::Middle)
        {
            panning_ = true;
            lastPixel_ = pixel;
            break;
        }

        const int node = graph_.nodeAt(toWorld(pixel), PICK_RADIUS);
        if (node == -1)
        {
            break;
        }
        if (event.mouseButton.button == sf::Mouse::Left && node != start_)
        {
            start_ = node;
            recompute();
        }
        else if (event.mouseButton.button == sf::Mouse::Right)
        {
            end_ = node;
        }
        break;
    }

    case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button == sf::Mouse::Middle)
        {
            panning_ = false;
        }
        break;

    case sf::Event::KeyPressed:
    {
        // Scancodes keep the shortcuts working on any keyboard layout.
        const sf::Vector2i pixel = sf::Mouse::getPosition(window_);
        switch (event.key.scancode)
        {
        case sf::Keyboard::Scan::F:
        {
            const int node = graph_.nodeAt(toWorld(pixel), PICK_RADIUS);
            if (node == -1)
            {
                break;
            }
            if (pendingLink_ == -1)
            {
                pendingLink_ = node;
            }
            else
            {
                if (pendingLink_ != node && graph_.addEdge(pendingLink_, node))
                {
                    recompute();
                }
                pendingLink_ = -1;
            }
            break;
        }
        case sf::Keyboard::Scan::A:
        {
            const sf::Vector2f position = toWorld(pixel);
            if (graph_.nodeAt(position, NODE_SPACING) == -1)
            {
                graph_.addNode(position);
                recompute();
                updateHover(pixel);
            }
            break;
        }
        case sf::Keyboard::Scan::H:
            showHelp_ = !showHelp_;
            break;
        case sf::Keyboard::Scan::Home:
            fitView();
            break;
        case sf::Keyboard::Scan::Escape:
            pendingLink_ = -1;
            break;
        default:
            break;
        }
        break;
    }

    default:
        break;
    }
}

void App::recompute()
{
    paths_ = dijkstra(graph_, start_);
}

void App::fitView()
{
    const auto& positions = graph_.positions();
    sf::Vector2f low = positions.front();
    sf::Vector2f high = positions.front();
    for (const sf::Vector2f& p : positions)
    {
        low.x = std::min(low.x, p.x);
        low.y = std::min(low.y, p.y);
        high.x = std::max(high.x, p.x);
        high.y = std::max(high.y, p.y);
    }

    // Keep badges and labels inside the window and clear of the side panel.
    const float margin = 140.f;
    const float width = static_cast<float>(window_.getSize().x);
    const float height = static_cast<float>(window_.getSize().y);
    const float reserved = width > 900.f ? PANEL_RESERVED_WIDTH : 0.f;

    const float zoomX = (high.x - low.x + 2.f * margin) / (width - reserved);
    const float zoomY = (high.y - low.y + 2.f * margin) / height;

    zoom_ = std::clamp(std::max(zoomX, zoomY), MIN_ZOOM, MAX_ZOOM);
    // Shift the centre so the graph sits in the middle of the free area.
    center_ = (low + high) / 2.f - sf::Vector2f(reserved / 2.f, 0.f) * zoom_;
    updateViews();
}

void App::updateViews()
{
    const sf::Vector2f size(static_cast<float>(window_.getSize().x), static_cast<float>(window_.getSize().y));

    screenView_ = sf::View(sf::FloatRect(0.f, 0.f, size.x, size.y));
    worldView_.setSize(size * zoom_);
    worldView_.setCenter(center_);
}

void App::zoomAt(sf::Vector2i pixel, float factor)
{
    const float newZoom = std::clamp(zoom_ * factor, MIN_ZOOM, MAX_ZOOM);
    if (newZoom == zoom_)
    {
        return;
    }

    // Keep the point under the cursor fixed while zooming.
    const sf::Vector2f before = toWorld(pixel);
    zoom_ = newZoom;
    updateViews();
    center_ += before - toWorld(pixel);
    updateViews();
    updateHover(pixel);
}

sf::Vector2f App::toWorld(sf::Vector2i pixel) const
{
    return window_.mapPixelToCoords(pixel, worldView_);
}

void App::updateHover(sf::Vector2i pixel)
{
    hovered_ = graph_.nodeAt(toWorld(pixel), PICK_RADIUS);
}

} // namespace dv
