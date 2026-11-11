
#pragma once

#include <cstdint>

namespace mapforge::core {

using OSMId = std::uint64_t;

}

// OSM identifiers can be large. A 64-bit unsigned integer provides a consistent type for node IDs, way IDs, and node references.