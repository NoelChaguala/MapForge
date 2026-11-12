
#include <gtest/gtest.h>

#include <vector>

#include "mapforge/graph/router.hpp"

namespace mapforge::graph {
namespace {

class RouterTest : public ::testing::Test {
protected:
    Graph graph;
    Router router;

    void add_vertex(core::OSMId id) {
        ASSERT_TRUE(graph.add_vertex(id));
    }

    void add_edge(
        core::OSMId source,
        core::OSMId destination,
        core::OSMId way_id,
        double distance
    ) {
        ASSERT_TRUE(graph.add_directed_edge(
            source,
            destination,
            way_id,
            distance,
            core::RoadMetadata{}
        ));
    }
};

TEST_F(RouterTest, FindsDirectRoute) {
    add_vertex(1);
    add_vertex(2);
    add_edge(1, 2, 100, 150.0);

    const auto result =
        router.find_shortest_path(graph, 1, 2);

    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.path, (std::vector<core::OSMId>{1, 2}));
    EXPECT_DOUBLE_EQ(result.total_distance_meters, 150.0);
}

TEST_F(RouterTest, FindsShortestRouteAmongMultiplePaths) {
    add_vertex(1);
    add_vertex(2);
    add_vertex(3);
    add_vertex(4);

    add_edge(1, 2, 100, 100.0);
    add_edge(2, 4, 101, 150.0);

    add_edge(1, 3, 102, 200.0);
    add_edge(3, 4, 103, 100.0);

    add_edge(1, 4, 104, 400.0);

    const auto result =
        router.find_shortest_path(graph, 1, 4);

    EXPECT_TRUE(result.found);
    EXPECT_EQ(
        result.path,
        (std::vector<core::OSMId>{1, 2, 4})
    );
    EXPECT_DOUBLE_EQ(result.total_distance_meters, 250.0);
}

TEST_F(RouterTest, ReturnsNoRouteWhenDestinationIsUnreachable) {
    add_vertex(1);
    add_vertex(2);
    add_vertex(3);

    add_edge(1, 2, 100, 100.0);

    const auto result =
        router.find_shortest_path(graph, 1, 3);

    EXPECT_FALSE(result.found);
    EXPECT_TRUE(result.path.empty());
}

TEST_F(RouterTest, RejectsInvalidStartingVertex) {
    add_vertex(2);

    const auto result =
        router.find_shortest_path(graph, 1, 2);

    EXPECT_FALSE(result.found);
    EXPECT_TRUE(result.path.empty());
}

TEST_F(RouterTest, RejectsInvalidDestinationVertex) {
    add_vertex(1);

    const auto result =
        router.find_shortest_path(graph, 1, 2);

    EXPECT_FALSE(result.found);
    EXPECT_TRUE(result.path.empty());
}

TEST_F(RouterTest, HandlesSameStartingAndDestinationVertex) {
    add_vertex(1);

    const auto result =
        router.find_shortest_path(graph, 1, 1);

    EXPECT_TRUE(result.found);
    EXPECT_EQ(result.path, (std::vector<core::OSMId>{1}));
    EXPECT_DOUBLE_EQ(result.total_distance_meters, 0.0);
}

TEST_F(RouterTest, RespectsDirectedEdges) {
    add_vertex(1);
    add_vertex(2);

    add_edge(1, 2, 100, 100.0);

    // There is no edge from vertex 2 back to vertex 1.
    const auto result =
        router.find_shortest_path(graph, 2, 1);

    EXPECT_FALSE(result.found);
    EXPECT_TRUE(result.path.empty());
}

TEST_F(RouterTest, UpdatesPreviouslyDiscoveredDistances) {
    add_vertex(1);
    add_vertex(2);
    add_vertex(3);
    add_vertex(4);

    // Initially, vertex 3 is discovered through vertex 2.
    add_edge(1, 2, 100, 10.0);
    add_edge(2, 3, 101, 100.0);

    // A shorter route to vertex 3 is discovered later.
    add_edge(1, 3, 102, 20.0);
    add_edge(3, 4, 103, 10.0);

    const auto result =
        router.find_shortest_path(graph, 1, 4);

    EXPECT_TRUE(result.found);
    EXPECT_EQ(
        result.path,
        (std::vector<core::OSMId>{1, 3, 4})
    );
    EXPECT_DOUBLE_EQ(result.total_distance_meters, 30.0);
}

TEST_F(RouterTest, SupportsZeroDistanceEdges) {
    add_vertex(1);
    add_vertex(2);
    add_vertex(3);

    add_edge(1, 2, 100, 0.0);
    add_edge(2, 3, 101, 50.0);

    const auto result =
        router.find_shortest_path(graph, 1, 3);

    EXPECT_TRUE(result.found);
    EXPECT_EQ(
        result.path,
        (std::vector<core::OSMId>{1, 2, 3})
    );
    EXPECT_DOUBLE_EQ(result.total_distance_meters, 50.0);
}

TEST_F(RouterTest, DoesNotModifyOriginalGraph) {
    add_vertex(1);
    add_vertex(2);
    add_edge(1, 2, 100, 100.0);

    const auto original_vertices = graph.vertex_count();
    const auto original_edges = graph.edge_count();

    (void)router.find_shortest_path(graph, 1, 2);

    EXPECT_EQ(graph.vertex_count(), original_vertices);
    EXPECT_EQ(graph.edge_count(), original_edges);
}

} // namespace
} // namespace mapforge::graph
