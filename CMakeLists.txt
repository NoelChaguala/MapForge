
cmake_minimum_required(VERSION 3.20)

project(MapForge
    VERSION 0.1.0
    DESCRIPTION "C++ Geospatial Processing Engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# pugixml configuration
set(PUGIXML_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(PUGIXML_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    pugixml
    GIT_REPOSITORY https://github.com/zeux/pugixml.git
    GIT_TAG v1.16
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(pugixml)

# Project options
option(MAPFORGE_BUILD_TESTS "Build MapForge tests" OFF)
option(MAPFORGE_BUILD_BENCHMARKS "Build MapForge benchmarks" OFF)

# GoogleTest configuration
if(MAPFORGE_BUILD_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.16.0
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(googletest)
endif()

# Compiler warnings
function(mapforge_enable_warnings target)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
        )
    endif()
endfunction()

# MapForge library
add_library(mapforge STATIC
    src/version.cpp
    src/osm/osm_parser.cpp
    
    src/graph/graph.cpp
    src/graph/graph_builder.cpp
    src/graph/nearby_search.cpp
    src/graph/router.cpp
    
    src/geography/geo_utils.cpp
)

add_library(MapForge::MapForge ALIAS mapforge)

target_include_directories(mapforge
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(mapforge PUBLIC cxx_std_20)

target_link_libraries(mapforge
    PRIVATE
        pugixml::pugixml
)

mapforge_enable_warnings(mapforge)

# Main application
add_executable(MapForge
    src/main.cpp
)

target_link_libraries(MapForge
    PRIVATE
        MapForge::MapForge
)

mapforge_enable_warnings(MapForge)

# Testing
if(MAPFORGE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Benchmarks
if(MAPFORGE_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
