
#pragma once

#include "mapforge/core/geo_point.hpp"

namespace mapforge::geography {

[[nodiscard]] bool is_valid_coordinate(
    const core::GeoPoint& point
) noexcept;

[[nodiscard]] double haversine_distance_meters(
    const core::GeoPoint& first,
    const core::GeoPoint& second
);

}
