
#include "mapforge/graph/nearby_search.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "mapforge/geography/geo_utils.hpp"

namespace mapforge::graph {

std::vector<NearbyIntersection> find_nearby_intersections(
    const Graph& graph,
    const std::vector<core::Node>& nodes,
    const core::GeoPoint& search_location,
    double radius_meters
) {
    // Validate search location.
    if (!geography::is_valid_coordinate(search_location)) {
        throw std::invalid_argument(
            "Search location contains invalid coordinates."
        );
    }

    // Validate search radius.
    if (!std::isfinite(radius_meters) || radius_meters < 0.0) {
        throw std::invalid_argument(
            "Search radius must be a finite, non-negative value."
        );
    }

    std::vector<NearbyIntersection> results;

    // Check each parsed OSM node.
    for (const core::Node& node : nodes) {

        // Only consider nodes that belong to the road graph.
        if (!graph.has_vertex(node.id)) {
            continue;
        }

        // Skip nodes with invalid geographic coordinates.
        if (!geography::is_valid_coordinate(node.location)) {
            continue;
        }

        // Calculate distance from the search location.
        const double distance =
            geography::haversine_distance_meters(
                search_location,
                node.location
            );

        // Include intersections within the requested radius.
        if (distance <= radius_meters) {
            results.push_back({
                node.id,
                distance
            });
        }
    }

    // Sort results from closest to farthest.
    std::sort(
        results.begin(),
        results.end(),
        [](const NearbyIntersection& first,
           const NearbyIntersection& second) {
            if (first.distance_meters != second.distance_meters) {
                return first.distance_meters < second.distance_meters;
            }

            return first.node_id < second.node_id;
        }
    );

    return results;
}

} // namespace mapforge::graph
