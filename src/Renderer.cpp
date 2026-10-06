#include "Renderer.hpp"

#include "Config.hpp"
#include "Theme.hpp"

#include <cmath>
#include <set>
#include <utility>

namespace dv
{

namespace
{

constexpr float PI = 3.14159265358979f;

sf::Color withAlpha(sf::Color color, sf::Uint8 alpha)
{
    color.a = alpha;
    return color;
}

std::string kilometres(int distance)
{
    return std::to_string(distance) + " km";
}

} // namespace

bool Renderer::loadFont(const std::string& path)
{
    if (!font_.loadFromFile(path))
    {
        return false;
    }
    text_.setFont(font_);
    return true;
}

void Renderer::drawLine(sf::Vector2f from, sf::Vector2f to, float thickness, sf::Color color)
{
    const sf::Vector2f delta = to - from;
    const float length = std::hypot(delta.x, delta.y);

    sf::RectangleShape rectangle({length, thickness});
    rectangle.setOrigin(0.f, thickness / 2.f);
    rectangle.setPosition(from);
    rectangle.setRotation(std::atan2(delta.y, delta.x) * 180.f / PI);
    rectangle.setFillColor(color);
    target_.draw(rectangle);
}

void Renderer::drawCircle(sf::Vector2f center, float radius, sf::Color fill, sf::Color outline, float outlineThickness)
{
    sf::CircleShape circle(radius, 48);
    circle.setOrigin(radius, radius);
    circle.setPosition(center);
    circle.setFillColor(fill);
    circle.setOutlineColor(outline);
    circle.setOutlineThickness(outlineThickness);
    target_.draw(circle);
}

void Renderer::drawLabel(const std::string& string, sf::Vector2f center, unsigned size, sf::Color color)
{
    text_.setString(string);
    text_.setCharacterSize(size);
    text_.setFillColor(color);
    const sf::FloatRect bounds = text_.getLocalBounds();
    text_.setOrigin(std::round(bounds.left + bounds.width / 2.f), std::round(bounds.top + bounds.height / 2.f));
    text_.setPosition(std::round(center.x), std::round(center.y));
    target_.draw(text_);
    text_.setOrigin(0.f, 0.f);
}

void Renderer::drawBadge(const std::string& string, sf::Vector2f center, sf::Color fill, sf::Color textColor)
{
    text_.setString(string);
    text_.setCharacterSize(18);
    const sf::FloatRect bounds = text_.getLocalBounds();

    sf::RectangleShape pill({bounds.width + 20.f, 28.f});
    pill.setOrigin(pill.getSize() / 2.f);
    pill.setPosition(center);
    pill.setFillColor(fill);
    target_.draw(pill);

    drawLabel(string, center, 18, textColor);
}

void Renderer::drawNode(const Scene& scene, int node)
{
    const sf::Vector2f position = scene.graph.position(node);
    const bool reachable = scene.paths.reachable(node);
    const bool isStart = node == scene.start;
    const bool isEnd = node == scene.end;

    sf::Color fill = reachable ? theme::NODE_FILL : theme::NODE_UNREACHABLE_FILL;
    sf::Color outline = reachable ? theme::NODE_OUTLINE : theme::NODE_UNREACHABLE_OUTLINE;
    sf::Color labelColor = reachable ? theme::TEXT : theme::TEXT_DIM;

    if (isStart || isEnd)
    {
        const sf::Color accent = isStart ? theme::START : theme::END;
        drawCircle(position, NODE_RADIUS + 9.f, withAlpha(accent, 40), sf::Color::Transparent, 0.f);
        fill = accent;
        outline = accent;
        labelColor = theme::BACKGROUND;
    }

    if (node == scene.pendingLink)
    {
        drawCircle(position, NODE_RADIUS + 9.f, withAlpha(theme::PENDING_LINK, 60), sf::Color::Transparent, 0.f);
        outline = theme::PENDING_LINK;
    }

    if (node == scene.hovered)
    {
        outline = theme::HOVER;
    }

    drawCircle(position, NODE_RADIUS, fill, outline, 3.f);
    drawLabel(std::to_string(node), position, 15, labelColor);
}

void Renderer::drawWorld(const Scene& scene)
{
    const Graph& graph = scene.graph;

    const std::vector<int> path = scene.paths.pathTo(scene.end);
    std::set<std::pair<int, int>> onPath;
    for (std::size_t i = 1; i < path.size(); ++i)
    {
        onPath.insert({std::min(path[i - 1], path[i]), std::max(path[i - 1], path[i])});
    }

    // Plain edges first, the shortest path on top of them.
    for (const Link& link : graph.links())
    {
        if (!onPath.count({std::min(link.a, link.b), std::max(link.a, link.b)}))
        {
            drawLine(graph.position(link.a), graph.position(link.b), 2.f, theme::EDGE);
        }
    }
    for (std::size_t i = 1; i < path.size(); ++i)
    {
        drawLine(graph.position(path[i - 1]), graph.position(path[i]), 6.f, theme::PATH);
    }

    // Edge weights sit on a small background so they stay readable over lines.
    for (const Link& link : graph.links())
    {
        const bool highlighted = onPath.count({std::min(link.a, link.b), std::max(link.a, link.b)}) > 0;
        const sf::Vector2f middle = (graph.position(link.a) + graph.position(link.b)) / 2.f;
        const std::string label = std::to_string(link.weight);

        text_.setString(label);
        text_.setCharacterSize(14);
        const sf::FloatRect bounds = text_.getLocalBounds();

        sf::RectangleShape pill({bounds.width + 12.f, 20.f});
        pill.setOrigin(pill.getSize() / 2.f);
        pill.setPosition(middle);
        pill.setFillColor(highlighted ? theme::PATH : theme::BACKGROUND);
        target_.draw(pill);

        drawLabel(label, middle, 14, highlighted ? theme::BACKGROUND : theme::EDGE_LABEL);
    }

    for (int node = 0; node < graph.nodeCount(); ++node)
    {
        drawNode(scene, node);
    }

    // Badges go last so they are never covered by other nodes.
    const float badgeOffset = NODE_RADIUS + 28.f;
    if (graph.isValid(scene.start))
    {
        drawBadge("START", graph.position(scene.start) - sf::Vector2f(0.f, badgeOffset), theme::START, theme::BACKGROUND);
    }
    if (graph.isValid(scene.end) && scene.end != scene.start)
    {
        const std::string label = scene.paths.reachable(scene.end)
                                      ? kilometres(scene.paths.distance[scene.end])
                                      : "unreachable";
        drawBadge(label, graph.position(scene.end) - sf::Vector2f(0.f, badgeOffset), theme::END, theme::BACKGROUND);
    }

    if (scene.hovered != -1 && scene.hovered != scene.start && scene.hovered != scene.end)
    {
        const std::string label = scene.paths.reachable(scene.hovered)
                                      ? kilometres(scene.paths.distance[scene.hovered])
                                      : "unreachable";
        drawBadge(label, graph.position(scene.hovered) - sf::Vector2f(0.f, badgeOffset), theme::PANEL_OUTLINE, theme::TEXT);
    }
}

void Renderer::drawHud(const Scene& scene)
{
    struct Row
    {
        std::string key;
        std::string value;
        sf::Color color;
    };

    const float left = 16.f;
    const float padding = 18.f;
    const float panelWidth = 340.f;
    const float keyColumn = 112.f;

    std::vector<Row> status;
    status.push_back({"Start", "#" + std::to_string(scene.start), theme::START});
    status.push_back({"End", "#" + std::to_string(scene.end), theme::END});
    if (scene.paths.reachable(scene.end))
    {
        status.push_back({"Distance", kilometres(scene.paths.distance[scene.end]), theme::PATH});
    }
    else
    {
        status.push_back({"Distance", "unreachable", theme::ERROR});
    }

    std::vector<Row> help;
    if (scene.showHelp)
    {
        help = {
            {"Left click", "set start", theme::TEXT},
            {"Right click", "set end", theme::TEXT},
            {"F  (twice)", "link two nodes", theme::TEXT},
            {"A", "add a node", theme::TEXT},
            {"Middle drag", "pan", theme::TEXT},
            {"Wheel", "zoom", theme::TEXT},
            {"Home", "fit view", theme::TEXT},
            {"Esc", "cancel linking", theme::TEXT},
            {"H", "hide help", theme::TEXT},
        };
    }

    const float titleHeight = 34.f;
    const float rowHeight = 26.f;
    float height = 2.f * padding + titleHeight + rowHeight * status.size();
    if (scene.pendingLink != -1)
    {
        height += rowHeight;
    }
    height += scene.showHelp ? 16.f + rowHeight * help.size() : 16.f + rowHeight;

    sf::RectangleShape panel({panelWidth, height});
    panel.setPosition(left, left);
    panel.setFillColor(theme::PANEL);
    panel.setOutlineColor(theme::PANEL_OUTLINE);
    panel.setOutlineThickness(1.f);
    target_.draw(panel);

    auto drawText = [&](const std::string& string, float x, float y, unsigned size, sf::Color color)
    {
        text_.setString(string);
        text_.setCharacterSize(size);
        text_.setFillColor(color);
        text_.setPosition(std::round(x), std::round(y));
        target_.draw(text_);
    };

    const float x = left + padding;
    float y = left + padding;

    drawText("Dijkstra's Algorithm", x, y, 22, theme::TEXT);
    y += titleHeight;

    for (const Row& row : status)
    {
        drawText(row.key, x, y, 18, theme::TEXT_DIM);
        drawText(row.value, x + keyColumn, y, 18, row.color);
        y += rowHeight;
    }

    if (scene.pendingLink != -1)
    {
        drawText("Linking #" + std::to_string(scene.pendingLink) + ": press F on another node", x, y, 16, theme::PENDING_LINK);
        y += rowHeight;
    }

    y += 16.f;
    if (scene.showHelp)
    {
        for (const Row& row : help)
        {
            drawText(row.key, x, y, 16, theme::TEXT);
            drawText(row.value, x + keyColumn, y, 16, theme::TEXT_DIM);
            y += rowHeight;
        }
    }
    else
    {
        drawText("H  show help", x, y, 16, theme::TEXT_DIM);
    }

    drawText(std::to_string(scene.graph.nodeCount()) + " nodes  |  " + std::to_string(scene.graph.edgeCount()) + " edges",
             left + padding, left + height + 10.f, 14, theme::TEXT_DIM);
}

} // namespace dv
