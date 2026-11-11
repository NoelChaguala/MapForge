
#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "mapforge/core/osm_id.hpp"
#include "mapforge/core/road_metadata.hpp"

namespace mapforge::core {

    struct Way {
        OSMId id{0};

        // Ordered OSM node IDs defining the way.
        std::vector<OSMId> node_references;

        // Normalized information used by MapForge.
        RoadMetadata metadata;

        // Original OSM tags preserved for future use.
        std::unordered_map<std::string, std::string> tags;
    };

}


// Way stores node IDs rather than copying entire Node objects. 
// This avoids unnecessary duplication and makes it easier to build a graph later.