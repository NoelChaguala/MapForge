
#include <filesystem>
#include <iostream>

#include "mapforge/osm/osm_parser.hpp"
#include "mapforge/version.hpp"

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

    return 0;
}
