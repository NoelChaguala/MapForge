
#pragma once

#include <string>
#include <unordered_map>

#include "mapforge/core/geo_point.hpp"
#include "mapforge/core/osm_id.hpp"

namespace mapforge::core {

struct Node {
    OSMId id{0};
    GeoPoint location;

    std::unordered_map<std::string, std::string> tags;
};

}

// A node contains{
//     - A unique OSM identifier.
//     - Geographic coordinates.
//     - Optional key-value tags.
// }
