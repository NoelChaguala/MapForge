
#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

#include "mapforge/graph/nearby_search.hpp"

namespace mapforge::graph {
namespace {

class NearbySearchTest : public ::testing::Test {
protected:
    Graph graph;
    std::vector<core::Node> nodes;

    const core::GeoPoint search_location{36.1627, -86.7816};

    void SetUp() override {
        // Add four intersections to the road graph.
        ASSERT_TRUE(graph.add_vertex(1));
        ASSERT_TRUE(graph.add_vertex(2));
        ASSERT_TRUE(graph.add_vertex(3));
        ASSERT_TRUE(graph.add_vertex(4));

        // Intersection 1: Search origin.
        nodes.push_back({
            1,
            {36.1627, -86.7816},
            {}
        });

        // Intersection 2: Approximately 500 meters away.
        nodes.push_back({
            2,
            {36.1672, -86.7816},
            {}
        });

        // Intersection 3: Approximately 1 kilometer away.
        nodes.push_back({
            3,
            {36.1717, -86.7816},
            {}
        });

        // Intersection 4: Approximately 2 kilometers away.
        nodes.push_back({
            4,
            {36.1807, -86.7816},
            {}
        });

        // Node 5 is parsed OSM data but not part of the graph.
        nodes.push_back({
            5,
            {36.1630, -86.7816},
            {}
        });
    }
};

// Test that nearby intersections are returned.
TEST_F(NearbySearchTest, ReturnsIntersectionsWithinRadius) {
    const auto results = find_nearby_intersections(
        graph,
        nodes,
        search_location,
        600.0
    );

    ASSERT_EQ(results.size(), 2);

    EXPECT_EQ(results[0].node_id, 1);
    EXPECT_EQ(results[1].node_id, 2);
}

// Test that intersections outside the radius are excluded.
TEST_F(NearbySearchTest, ExcludesIntersectionsOutsideRadius) {
    const auto results = find_nearby_intersections(
        graph,
        nodes,
        search_location,
        600.0
    );

    for (const auto& result : results) {
        EXPECT_LE(result.distance_meters, 600.0);
    }

    EXPECT_EQ(results.size(), 2);
}

// Test that distances are calculated correctly.
TEST_F(NearbySearchTest, CalculatesDistancesCorrectly) {
    const auto results = find_nearby_intersections(
        graph,
        nodes,
        search_location,
        600.0
    );

    ASSERT_EQ(results.size(), 2);

    // The search origin should have zero distance.
    EXPECT_NEAR(results[0].distance_meters, 0.0, 0.001);

    // The second intersection is approximately 500 meters away.
    EXPECT_NEAR(results[1].distance_meters, 500.4, 1.0);
}

// Test that results are sorted from closest to farthest.
TEST_F(NearbySearchTest, SortsResultsByDistance) {
    const auto results = find_nearby_intersections(
        graph,
        nodes,
        search_location,
        3000.0
    );

    ASSERT_EQ(results.size(), 4);

    for (std::size_t i = 1; i < results.size(); ++i) {
        EXPECT_LE(
            results[i - 1].distance_meters,
            results[i].distance_meters
        );
    }
}

// Test that parsed nodes not belonging to the graph are excluded.
TEST_F(NearbySearchTest, ExcludesNodesNotInGraph) {
    const auto results = find_nearby_intersections(
        graph,
        nodes,
        search_location,
        1000.0
    );

    for (const auto& result : results) {
        EXPECT_NE(result.node_id, 5);
    }
}

// Test invalid latitude.
TEST_F(NearbySearchTest, RejectsInvalidLatitude) {
    const core::GeoPoint invalid_location{91.0, -86.7816};

    EXPECT_THROW(
        find_nearby_intersections(
            graph,
            nodes,
            invalid_location,
            1000.0
        ),
        std::invalid_argument
    );
}

// Test invalid longitude.
TEST_F(NearbySearchTest, RejectsInvalidLongitude) {
    const core::GeoPoint invalid_location{36.1627, 181.0};

    EXPECT_THROW(
        find_nearby_intersections(
            graph,
            nodes,
            invalid_location,
            1000.0
        ),
        std::invalid_argument
    );
}

// Test negative search radius.
TEST_F(NearbySearchTest, RejectsNegativeRadius) {
    EXPECT_THROW(
        find_nearby_intersections(
            graph,
            nodes,
            search_location,
            -100.0
        ),
        std::invalid_argument
    );
}

// Test infinite search radius.
TEST_F(NearbySearchTest, RejectsInfiniteRadius) {
    EXPECT_THROW(
        find_nearby_intersections(
            graph,
            nodes,
            search_location,
            std::numeric_limits<double>::infinity()
        ),
        std::invalid_argument
    );
}

// Test NaN search radius.
TEST_F(NearbySearchTest, RejectsNaNRadius) {
    EXPECT_THROW(
        find_nearby_intersections(
            graph,
            nodes,
            search_location,
            std::numeric_limits<double>::quiet_NaN()
        ),
        std::invalid_argument
    );
}

// Test zero-radius search.
TEST_F(NearbySearchTest, FindsExactLocationWithZeroRadius) {
    const auto results = find_nearby_intersections(
        graph,
        nodes,
        search_location,
        0.0
    );

    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].node_id, 1);
    EXPECT_DOUBLE_EQ(results[0].distance_meters, 0.0);
}

// Test when no intersections are within the radius.
TEST_F(NearbySearchTest, ReturnsEmptyResultsWhenNoIntersectionsMatch) {
    const core::GeoPoint distant_location{0.0, 0.0};

    const auto results = find_nearby_intersections(
        graph,
        nodes,
        distant_location,
        100.0
    );

    EXPECT_TRUE(results.empty());
}

// Test that invalid coordinates in the dataset are skipped.
TEST_F(NearbySearchTest, SkipsNodesWithInvalidCoordinates) {
    nodes.push_back({
        6,
        {91.0, -86.7816},
        {}
    });

    ASSERT_TRUE(graph.add_vertex(6));

    const auto results = find_nearby_intersections(
        graph,
        nodes,
        search_location,
        1000.0
    );

    for (const auto& result : results) {
        EXPECT_NE(result.node_id, 6);
    }
}

} // namespace
} // namespace mapforge::graph
