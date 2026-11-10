
cmake_minimum_required(VERSION 3.20)

project(MapForge
    VERSION 0.1.0
    DESCRIPTION "C++ Geospatial Processing Engine"
    LANGUAGES CXX
)

# C++ configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(MAPFORGE_BUILD_TESTS "Build MapForge tests" OFF)
option(MAPFORGE_BUILD_BENCHMARKS "Build MapForge benchmarks" OFF)

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

# Reusable MapForge library
add_library(mapforge STATIC
    src/version.cpp
)

add_library(MapForge::MapForge ALIAS mapforge)

target_include_directories(mapforge
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(mapforge PUBLIC cxx_std_20)

mapforge_enable_warnings(mapforge)

# MapForge executable
add_executable(MapForge
    src/main.cpp
)

target_link_libraries(MapForge
    PRIVATE
        MapForge::MapForge
)

mapforge_enable_warnings(MapForge)

# Optional tests
if(MAPFORGE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Optional benchmarks
if(MAPFORGE_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
