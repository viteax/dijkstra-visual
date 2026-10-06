#pragma once

namespace dv
{

// Size of the world in which random graphs are generated.
constexpr float WORLD_WIDTH = 2400.f;
constexpr float WORLD_HEIGHT = 1500.f;

constexpr float NODE_RADIUS = 15.f;
// Minimum distance between two nodes when generating or adding them by hand.
constexpr float NODE_SPACING = 3.f * NODE_RADIUS;
// How close the cursor has to be to a node to pick it.
constexpr float PICK_RADIUS = 1.6f * NODE_RADIUS;

// Width of the HUD panel plus its margin; "fit view" keeps the graph clear of it.
constexpr float PANEL_RESERVED_WIDTH = 372.f;

constexpr float MIN_ZOOM = 0.15f; // world units per pixel
constexpr float MAX_ZOOM = 8.f;

} // namespace dv
