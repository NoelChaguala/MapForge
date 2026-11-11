
#include "mapforge/geography/geo_utils.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace mapforge::geography {

namespace {

constexpr double earth_radius_meters = 6'371'000.0;
constexpr double pi = 3.14159265358979323846;

[[nodiscard]] double to_radians(double degrees) noexcept {
    return degrees * pi / 180.0;
}

}

bool is_valid_coordinate(const core::GeoPoint& point) noexcept {
    return std::isfinite(point.latitude) &&
           std::isfinite(point.longitude) &&
           point.latitude >= -90.0 &&
           point.latitude <= 90.0 &&
           point.longitude >= -180.0 &&
           point.longitude <= 180.0;
}

double haversine_distance_meters(
    const core::GeoPoint& first,
    const core::GeoPoint& second
) {
    if (!is_valid_coordinate(first) || !is_valid_coordinate(second)) {
        throw std::invalid_argument(
            "Cannot calculate distance using invalid geographic coordinates."
        );
    }

    const double latitude1 = to_radians(first.latitude);
    const double latitude2 = to_radians(second.latitude);

    const double latitude_difference = latitude2 - latitude1;

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

    const double clamped_a = std::clamp(a, 0.0, 1.0);

    const double central_angle =
        2.0 * std::asin(std::sqrt(clamped_a));

    return earth_radius_meters * central_angle;
}

}
