#pragma once

#include "Graph.hpp"

#include <random>
#include <string>

namespace dv
{

// Fills `graph` with `count` random nodes and a connected random set of links.
void generateRandom(Graph& graph, int count, std::mt19937& rng);

// Reads "x y" pairs, one node per line. Returns false if the file can't be opened.
bool loadNodes(const std::string& path, Graph& graph);

// Reads "a b" pairs of node indices, one link per line. Invalid lines are
// reported on stderr and skipped. Returns false if the file can't be opened.
bool loadLinks(const std::string& path, Graph& graph);

// Looks for `relativePath` in the working directory and then in the source tree.
std::string findResource(const std::string& relativePath);

} // namespace dv
