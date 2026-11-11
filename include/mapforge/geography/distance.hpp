
#pragma once

#include "mapforge/core/geo_point.hpp"

namespace mapforge::geography {

[[nodiscard]] double haversine_distance_meters(
    const core::GeoPoint& first,
    const core::GeoPoint& second
) noexcept;

} // namespace mapforge::geography
