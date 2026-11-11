
#pragma once

#include <filesystem>
#include <string>

namespace mapforge::osm {

enum class OSMParseError {
    none,
    file_open_failed,
    invalid_xml,
    invalid_root,
    read_failed
};

struct OSMParseResult {
    OSMParseError error{OSMParseError::none};
    std::string message;

    [[nodiscard]] bool success() const noexcept {
        return error == OSMParseError::none;
    }
};

class OSMParser {
public:
    [[nodiscard]] OSMParseResult load_file(
        const std::filesystem::path& file_path
    ) const;
};

}
