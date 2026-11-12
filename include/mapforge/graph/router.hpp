
#pragma once

#include <vector>

#include "mapforge/core/osm_id.hpp"
#include "mapforge/graph/graph.hpp"

namespace mapforge::graph {

struct RouteResult {
    bool found{false};

    std::vector<core::OSMId> path;

    double total_distance_meters{0.0};
};

class Router {
public:
    // Calculates the shortest-distance route between two vertices.
    [[nodiscard]] RouteResult find_shortest_path(
        const Graph& graph,
        core::OSMId start,
        core::OSMId destination
    ) const;
};

} // namespace mapforge::graph

//