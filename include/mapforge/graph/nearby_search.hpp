
#pragma once

#include <vector>

#include "mapforge/core/geo_point.hpp"
#include "mapforge/core/node.hpp"
#include "mapforge/graph/graph.hpp"

namespace mapforge::graph {

struct NearbyIntersection {
    core::OSMId node_id{0};
    double distance_meters{0.0};
};

// Finds graph intersections within the specified geographic radius.
// Results are sorted by distance, closest first.
//
// Throws std::invalid_argument if the search coordinate is invalid
// or the search radius is negative or non-finite.
[[nodiscard]] std::vector<NearbyIntersection>
find_nearby_intersections(
    const Graph& graph,
    const std::vector<core::Node>& nodes,
    const core::GeoPoint& search_location,
    double radius_meters
);

} // namespace mapforge::graph
