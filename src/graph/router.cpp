
#include "mapforge/graph/router.hpp"

#include <algorithm>
#include <functional>
#include <queue>
#include <unordered_map>
#include <utility>

namespace mapforge::graph {

RouteResult Router::find_shortest_path(
    const Graph& graph,
    core::OSMId start,
    core::OSMId destination
) const {

    RouteResult result;

    // Both endpoints must exist in the graph.
    if (!graph.has_vertex(start) ||
        !graph.has_vertex(destination)) {
        return result;
    }

    // A route from a vertex to itself has zero distance.
    if (start == destination) {
        result.found = true;
        result.path.push_back(start);
        return result;
    }

    using QueueEntry = std::pair<double, core::OSMId>;

    // Min-priority queue: smallest distance is processed first.
    std::priority_queue<
        QueueEntry,
        std::vector<QueueEntry>,
        std::greater<QueueEntry>
    > priority_queue;

    // Store the shortest known distance to each vertex.
    std::unordered_map<core::OSMId, double> distances;

    // Track the previous vertex for path reconstruction.
    std::unordered_map<core::OSMId, core::OSMId> previous;

    distances.emplace(start, 0.0);
    priority_queue.emplace(0.0, start);

    // Process vertices until the destination is finalized.
    while (!priority_queue.empty()) {

        const auto [current_distance, current_vertex] =
            priority_queue.top();

        priority_queue.pop();

        const auto distance_iterator =
            distances.find(current_vertex);

        // Ignore outdated queue entries.
        if (distance_iterator == distances.end() ||
            current_distance > distance_iterator->second) {
            continue;
        }

        // The shortest distance to the destination is finalized.
        if (current_vertex == destination) {
            break;
        }

        // Retrieve outgoing directed edges.
        const auto* outgoing_edges =
            graph.neighbors(current_vertex);

        if (outgoing_edges == nullptr) {
            continue;
        }

        for (const Edge& edge : *outgoing_edges) {

            const double new_distance =
                current_distance + edge.distance_meters;

            const auto neighbor_iterator =
                distances.find(edge.destination);

            // Relax the edge if a shorter route is discovered.
            if (neighbor_iterator == distances.end() ||
                new_distance < neighbor_iterator->second) {

                distances[edge.destination] = new_distance;

                previous[edge.destination] = current_vertex;

                priority_queue.emplace(
                    new_distance,
                    edge.destination
                );
            }
        }
    }

    // No route was found.
    const auto destination_iterator =
        distances.find(destination);

    if (destination_iterator == distances.end()) {
        return result;
    }

    // Reconstruct the route from destination to start.
    core::OSMId current_vertex = destination;

    result.path.push_back(current_vertex);

    while (current_vertex != start) {

        const auto previous_iterator =
            previous.find(current_vertex);

        // Protect against an incomplete predecessor chain.
        if (previous_iterator == previous.end()) {
            return RouteResult{};
        }

        current_vertex = previous_iterator->second;

        result.path.push_back(current_vertex);
    }

    // Restore the route to start-to-destination order.
    std::reverse(
        result.path.begin(),
        result.path.end()
    );

    result.total_distance_meters =
        destination_iterator->second;

    result.found = true;

    return result;
}

} // namespace mapforge::graph
