
#pragma once

#include <string>

namespace mapforge::core {

    enum class RoadDirection {
        bidirectional,
        forward,
        reverse,
        unknown
    };

    struct RoadMetadata {
        std::string name;
        std::string highway_type;

        RoadDirection direction{
            RoadDirection::bidirectional
        };
    };

}

