
#include "mapforge/geography/distance.hpp"

#include <algorithm>
#include <cmath>

namespace mapforge::geography {

namespace {

constexpr double earth_radius_meters = 6'371'000.0;

[[nodiscard]] double to_radians(double degrees) noexcept {
    constexpr double pi = 3.14159265358979323846;
    return degrees * pi / 180.0;
}

} // namespace

double haversine_distance_meters(
    const core::GeoPoint& first,
    const core::GeoPoint& second
) noexcept {
    const double latitude1 = to_radians(first.latitude);
    const double latitude2 = to_radians(second.latitude);

    const double latitude_difference =
        latitude2 - latitude1;

    const double longitude_difference =
        to_radians(second.longitude - first.longitude);

    const double sin_latitude =
        std::sin(latitude_difference / 2.0);

    const double sin_longitude =
        std::sin(longitude_difference / 2.0);

    const double a =
        sin_latitude * sin_latitude +
        std::cos(latitude1) * std::cos(latitude2) *
        sin_longitude * sin_longitude;

    // Protect against small floating-point rounding errors.
    const double clamped_a = std::clamp(a, 0.0, 1.0);

    const double central_angle =
        2.0 * std::asin(std::sqrt(clamped_a));

    return earth_radius_meters * central_angle;
}

} // namespace mapforge::geography
