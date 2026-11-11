
#pragma once

#include <cstddef>
#include <unordered_map>
#include <vector>

#include "mapforge/core/osm_id.hpp"
#include "mapforge/core/road_metadata.hpp"

namespace mapforge::graph {

struct Edge {
    core::OSMId destination{0};
    core::OSMId way_id{0};
    double distance_meters{0.0};
    core::RoadMetadata road_metadata;
};

class Graph {
public:
    // Adds a vertex. Returns false if the ID is invalid or already exists.
    [[nodiscard]] bool add_vertex(core::OSMId node_id);

    // Adds a directed edge between two existing vertices.
    // Returns false if the edge is invalid or already exists.
    [[nodiscard]] bool add_directed_edge(
        core::OSMId source,
        core::OSMId destination,
        core::OSMId way_id,
        double distance_meters,
        const core::RoadMetadata& metadata
    );

    // Returns nullptr if the vertex does not exist.
    [[nodiscard]] const std::vector<Edge>* neighbors(
        core::OSMId node_id
    ) const noexcept;

    [[nodiscard]] bool has_vertex(core::OSMId node_id) const noexcept;

    [[nodiscard]] std::size_t vertex_count() const noexcept;

    [[nodiscard]] std::size_t edge_count() const noexcept;

private:
    std::unordered_map<core::OSMId, std::vector<Edge>> adjacency_list_;
    std::size_t edge_count_{0};
};

} // namespace mapforge::graph
