# GoogleTest targets will be configured here.


# Nearby intersection search tests
add_executable(nearby_search_test
    graph/nearby_search_test.cpp
)

target_link_libraries(nearby_search_test
    PRIVATE
        MapForge::MapForge
        GTest::gtest_main
)

# Router tests
add_executable(router_test
    graph/router_test.cpp
)

target_link_libraries(router_test
    PRIVATE
        MapForge::MapForge
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(nearby_search_test) # Nearby intersection search tests
gtest_discover_tests(router_test) # Router tests
