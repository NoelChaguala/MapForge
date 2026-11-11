
#pragma once

#include <vector>

#include "mapforge/core/osm_id.hpp"
#include "mapforge/core/road_metadata.hpp"

namespace mapforge::core {

struct Way {
    OSMId id{0};

    std::vector<OSMId> node_references;

    RoadMetadata metadata;
};

}


// Way stores node IDs rather than copying entire Node objects. 
// This avoids unnecessary duplication and makes it easier to build a graph later.