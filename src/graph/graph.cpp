
#include "mapforge/graph/graph.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

namespace mapforge::graph {

bool Graph::add_vertex(core::OSMId node_id) {
    if (node_id == 0) {
        return false;
    }

    const auto [iterator, inserted] =
        adjacency_list_.try_emplace(node_id);

    (void)iterator;
    return inserted;
}

bool Graph::add_directed_edge(
    core::OSMId source,
    core::OSMId destination,
    core::OSMId way_id,
    double distance_meters,
    const core::RoadMetadata& metadata
) {
    // Reject invalid IDs and distances.
    if (source == 0 || destination == 0 || way_id == 0 ||
        !std::isfinite(distance_meters) || distance_meters < 0.0) {
        return false;
    }

    // Both vertices must already exist.
    const auto source_iterator = adjacency_list_.find(source);
    const auto destination_iterator = adjacency_list_.find(destination);

    if (source_iterator == adjacency_list_.end() ||
        destination_iterator == adjacency_list_.end()) {
        return false;
    }

    auto& outgoing_edges = source_iterator->second;

    // Prevent duplicate connections from the same OSM way.
    // Different ways may connect the same pair of vertices.
    const bool duplicate = std::any_of(
        outgoing_edges.begin(),
        outgoing_edges.end(),
        [destination, way_id](const Edge& edge) {
            return edge.destination == destination &&
                   edge.way_id == way_id;
        }
    );

    if (duplicate) {
        return false;
    }

    outgoing_edges.push_back({
        destination,
        way_id,
        distance_meters,
        metadata
    });

    ++edge_count_;
    return true;
}

const std::vector<Edge>* Graph::neighbors(
    core::OSMId node_id
) const noexcept {
    const auto iterator = adjacency_list_.find(node_id);

    if (iterator == adjacency_list_.end()) {
        return nullptr;
    }

    return &iterator->second;
}

bool Graph::has_vertex(core::OSMId node_id) const noexcept {
    return adjacency_list_.contains(node_id);
}

std::size_t Graph::vertex_count() const noexcept {
    return adjacency_list_.size();
}

std::size_t Graph::edge_count() const noexcept {
    return edge_count_;
}

} // namespace mapforge::graph
