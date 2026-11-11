
#include <filesystem>
#include <iostream>

#include "mapforge/osm/osm_parser.hpp"
#include "mapforge/version.hpp"

namespace {

const char* direction_to_string(
    mapforge::core::RoadDirection direction
) {
    using mapforge::core::RoadDirection;

    switch (direction) {
        case RoadDirection::forward:
            return "Forward";

        case RoadDirection::reverse:
            return "Reverse";

        case RoadDirection::bidirectional:
            return "Bidirectional";

        case RoadDirection::unknown:
            return "Unknown";
    }

    return "Unknown";
}

}

int main(int argc, char* argv[]) {

    std::cout << "================================\n";
    std::cout << "       " << mapforge::project_name()
              << " v" << mapforge::version() << '\n';
    std::cout << "================================\n";

    if (argc < 2) {
        std::cout << "Usage: MapForge <osm-file>\n";
        return 0;
    }

    const std::filesystem::path file_path{argv[1]};

    const mapforge::osm::OSMParser parser;
    const auto result = parser.load_file(file_path);

    if (!result.success()) {
        std::cerr << "OSM parsing failed: "
                  << result.message << '\n';

        return 1;
    }

    std::cout << result.message << '\n';

    // Display every parsed road.
    for (const auto& way : result.ways) {

        std::cout << "\nRoad:\n";
        std::cout << "Way ID: " << way.id << '\n';

        std::cout << "Name: "
                  << (way.metadata.name.empty()
                      ? "Unnamed"
                      : way.metadata.name)
                  << '\n';

        std::cout << "Highway type: "
                  << way.metadata.highway_type << '\n';

        std::cout << "Direction: "
                  << direction_to_string(way.metadata.direction)
                  << '\n';

        std::cout << "Node references: ";

        for (const auto reference : way.node_references) {
            std::cout << reference << ' ';
        }

        std::cout << '\n';

        std::cout << "Preserved tags: "
                  << way.tags.size() << '\n';
    }

    // Display unresolved node references.
    if (!result.missing_node_references.empty()) {

        std::cout << "\nWarning: "
                  << result.missing_node_references.size()
                  << " unresolved node references found.\n";

        for (const auto reference :
             result.missing_node_references) {

            std::cout << "Missing node ID: "
                      << reference << '\n';
        }
    }

    return 0;
}
