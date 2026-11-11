
#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "mapforge/core/node.hpp"
#include "mapforge/core/osm_id.hpp"
#include "mapforge/core/way.hpp"

namespace mapforge::osm {

enum class OSMParseError {
    none,
    file_open_failed,
    invalid_xml,
    invalid_root,
    read_failed,
    malformed_data
};

struct OSMParseResult {
    OSMParseError error{OSMParseError::none};
    std::string message;

    // Parsed geographic data.
    std::vector<core::Node> nodes;
    std::vector<core::Way> ways;

    // Unique OSM IDs referenced by ways but not found
    // in the parsed node collection.
    std::vector<core::OSMId> missing_node_references;

    [[nodiscard]] bool success() const noexcept {
        return error == OSMParseError::none;
    }
};

class OSMParser {
public:
    [[nodiscard]] OSMParseResult load_file(
        const std::filesystem::path& file_path
    ) const;
};

}

// OSMParser is responsible for parsing OpenStreetMap (OSM) XML files and extracting geographic data into structured formats.
// It reads nodes and ways, preserving their relationships and metadata, while also identifying any missing node references
// that are required by the ways.