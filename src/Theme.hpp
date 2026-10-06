#pragma once

#include <SFML/Graphics/Color.hpp>

namespace dv::theme
{

const sf::Color BACKGROUND(17, 19, 27);
const sf::Color GRID(255, 255, 255, 9);

const sf::Color EDGE(96, 106, 135, 150);
const sf::Color EDGE_LABEL(139, 148, 176);
const sf::Color PATH(64, 224, 168);

const sf::Color RELAX(255, 159, 67);
const sf::Color SETTLED_FILL(28, 88, 74);

const sf::Color NODE_FILL(38, 43, 61);
const sf::Color NODE_OUTLINE(122, 133, 168);
const sf::Color NODE_UNREACHABLE_FILL(28, 31, 43);
const sf::Color NODE_UNREACHABLE_OUTLINE(70, 76, 98);

const sf::Color START(255, 196, 61);
const sf::Color END(92, 170, 255);
const sf::Color PENDING_LINK(214, 112, 255);
const sf::Color HOVER(255, 255, 255);
const sf::Color ERROR(255, 99, 112);

const sf::Color TEXT(232, 236, 247);
const sf::Color TEXT_DIM(139, 148, 176);
const sf::Color PANEL(24, 27, 39, 235);
const sf::Color PANEL_OUTLINE(58, 64, 88);

} // namespace dv::theme
