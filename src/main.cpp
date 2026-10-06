#include "App.hpp"
#include "GraphIO.hpp"

#include <iostream>
#include <random>
#include <string>

namespace
{

void printUsage(const char* program)
{
    std::cout << "Usage: " << program << " [options]\n"
              << "\n"
              << "Without options the program asks how to build the graph.\n"
              << "\n"
              << "Options:\n"
              << "  --random N        generate a random graph with N nodes\n"
              << "  --file            load the graph from data/points.txt and data/graph.txt\n"
              << "  --points PATH     file with node coordinates (implies --file)\n"
              << "  --graph PATH      file with links between nodes (implies --file)\n"
              << "  -h, --help        show this message\n";
}

struct Options
{
    bool random = false;
    bool file = false;
    int count = 0;
    std::string pointsPath = "data/points.txt";
    std::string graphPath = "data/graph.txt";
    bool help = false;
    bool valid = true;
};

Options parseArguments(int argc, char** argv)
{
    Options options;
    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];
        const bool hasValue = i + 1 < argc;

        if (arg == "-h" || arg == "--help")
        {
            options.help = true;
        }
        else if (arg == "--file")
        {
            options.file = true;
        }
        else if (arg == "--random" && hasValue)
        {
            options.random = true;
            options.count = std::atoi(argv[++i]);
        }
        else if (arg == "--points" && hasValue)
        {
            options.file = true;
            options.pointsPath = argv[++i];
        }
        else if (arg == "--graph" && hasValue)
        {
            options.file = true;
            options.graphPath = argv[++i];
        }
        else
        {
            std::cerr << "Unknown or incomplete option: " << arg << "\n\n";
            options.valid = false;
        }
    }
    if (options.random && options.file)
    {
        std::cerr << "--random can't be combined with file options\n\n";
        options.valid = false;
    }
    return options;
}

// Asks on the console how the graph should be built.
void askInteractively(Options& options)
{
    std::cout << "Dijkstra's Algorithm Visualization\n"
              << "Build the graph from a file (F) or at random (R)?\n";

    std::string answer;
    while (answer != "R" && answer != "r" && answer != "F" && answer != "f")
    {
        std::cout << "Choice [R/F]: ";
        if (!(std::cin >> answer))
        {
            options.valid = false;
            return;
        }
    }

    if (answer == "R" || answer == "r")
    {
        options.random = true;
        while (options.count < 2)
        {
            std::cout << "Number of points (at least 2): ";
            if (!(std::cin >> options.count))
            {
                std::cin.clear();
                std::cin.ignore(1 << 16, '\n');
                options.count = 0;
            }
        }
    }
    else
    {
        options.file = true;
    }
}

} // namespace

int main(int argc, char** argv)
{
    Options options = parseArguments(argc, argv);
    if (options.help || !options.valid)
    {
        printUsage(argv[0]);
        return options.valid ? 0 : 1;
    }

    if (!options.random && !options.file)
    {
        askInteractively(options);
        if (!options.valid)
        {
            return 1;
        }
    }

    dv::Graph graph;

    if (options.random)
    {
        if (options.count < 2)
        {
            std::cerr << "A random graph needs at least 2 points.\n";
            return 1;
        }
        std::mt19937 rng{std::random_device{}()};
        dv::generateRandom(graph, options.count, rng);
    }
    else
    {
        const std::string pointsPath = dv::findResource(options.pointsPath);
        const std::string graphPath = dv::findResource(options.graphPath);

        if (!dv::loadNodes(pointsPath, graph))
        {
            std::cerr << "Can't open " << pointsPath << '\n';
            return 1;
        }
        if (!dv::loadLinks(graphPath, graph))
        {
            std::cerr << "Can't open " << graphPath << '\n';
            return 1;
        }
    }

    if (graph.empty())
    {
        std::cerr << "The graph has no nodes.\n";
        return 1;
    }

    dv::App app(std::move(graph));
    return app.run();
}
