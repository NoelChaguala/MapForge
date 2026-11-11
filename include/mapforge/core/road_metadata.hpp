#pragma once

#include <string>

namespace mapforge::core {

struct RoadMetadata {
    std::string name;
    std::string highway_type;
    bool one_way{false};
};

}

// Road metadata contains information about the road's name, type, and directionality.