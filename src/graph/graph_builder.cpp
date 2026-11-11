
#include "mapforge/graph/graph_builder.hpp"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "mapforge/geography/distance.hpp"

namespace mapforge::graph {

GraphBuildResult GraphBuilder::build(
    const osm::OSMParseResult& parsed_data
) const {
    GraphBuildResult result;

    // Do not build a graph if OSM parsing failed.
    if (!parsed_data.success()) {
        result.success = false;
        result.message =
            "Cannot build graph from unsuccessful OSM parsing.";
        return result;
    }

    // Create a lookup table for efficient OSM node access.
    std::unordered_map<core::OSMId, const core::Node*> node_lookup;
    node_lookup.reserve(parsed_data.nodes.size());

    for (const auto& node : parsed_data.nodes) {
        if (node.id != 0) {
            node_lookup.try_emplace(node.id, &node);
        }
    }

    // Track missing node references without duplicates.
    std::unordered_set<core::OSMId> missing_references;

    // Process each parsed highway way.
    for (const auto& way : parsed_data.ways) {

        // Record missing references for every parsed highway way.
        for (const auto node_id : way.node_references) {
            if (!node_lookup.contains(node_id)) {
                missing_references.insert(node_id);
            }
        }

        // Skip ways whose direction cannot be interpreted safely.
        if (way.metadata.direction == core::RoadDirection::unknown) {
            ++result.skipped_unknown_direction_ways;
            continue;
        }

        // Add valid referenced nodes as graph vertices.
        for (const auto node_id : way.node_references) {
            if (node_lookup.contains(node_id)) {
                (void)result.graph.add_vertex(node_id);
            }
        }

        // Connect consecutive nodes belonging to the same way.
        for (std::size_t i = 0;
             i + 1 < way.node_references.size();
             ++i) {

            const core::OSMId first_id = way.node_references[i];
            const core::OSMId second_id = way.node_references[i + 1];

            const auto first_node = node_lookup.find(first_id);
            const auto second_node = node_lookup.find(second_id);

            // Skip connections when either referenced node is missing.
            if (first_node == node_lookup.end() ||
                second_node == node_lookup.end()) {
                ++result.skipped_segments;
                continue;
            }

            // Calculate the distance between consecutive nodes.
            const double distance =
                geography::haversine_distance_meters(
                    first_node->second->location,
                    second_node->second->location
                );

            // Create directed edges according to road direction.
            switch (way.metadata.direction) {

                case core::RoadDirection::forward:
                    (void)result.graph.add_directed_edge(
                        first_id,
                        second_id,
                        way.id,
                        distance,
                        way.metadata
                    );
                    break;

                case core::RoadDirection::reverse:
                    (void)result.graph.add_directed_edge(
                        second_id,
                        first_id,
                        way.id,
                        distance,
                        way.metadata
                    );
                    break;

                case core::RoadDirection::bidirectional:
                    (void)result.graph.add_directed_edge(
                        first_id,
                        second_id,
                        way.id,
                        distance,
                        way.metadata
                    );

                    (void)result.graph.add_directed_edge(
                        second_id,
                        first_id,
                        way.id,
                        distance,
                        way.metadata
                    );
                    break;

                case core::RoadDirection::unknown:
                    // Unknown directions are handled before this loop.
                    break;
            }
        }
    }

    // Store missing references in deterministic order.
    result.missing_node_references.assign(
        missing_references.begin(),
        missing_references.end()
    );

    std::sort(
        result.missing_node_references.begin(),
        result.missing_node_references.end()
    );

    result.message = "Graph construction completed.";

    return result;
}

} // namespace mapforge::graph
