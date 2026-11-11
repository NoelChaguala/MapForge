
#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "mapforge/graph/graph.hpp"
#include "mapforge/osm/osm_parser.hpp"

namespace mapforge::graph {

struct GraphBuildResult {
    Graph graph;

    std::size_t skipped_segments{0};
    std::size_t skipped_unknown_direction_ways{0};

    std::vector<core::OSMId> missing_node_references;

    bool success{true};
    std::string message;
};

class GraphBuilder {
public:
    [[nodiscard]] GraphBuildResult build(
        const osm::OSMParseResult& parsed_data
    ) const;
};

} // namespace mapforge::graph
