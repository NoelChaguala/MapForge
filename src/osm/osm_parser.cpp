
#include "mapforge/osm/osm_parser.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <fstream>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_set>
#include <utility>

#include <pugixml.hpp>

namespace mapforge::osm {

namespace {

// Parse a positive OSM ID.
bool parse_osm_id(
    std::string_view value,
    core::OSMId& id
) {
    if (value.empty()) {
        return false;
    }

    core::OSMId parsed_id{0};

    const auto result = std::from_chars(
        value.data(),
        value.data() + value.size(),
        parsed_id
    );

    if (result.ec != std::errc{} ||
        result.ptr != value.data() + value.size() ||
        parsed_id == 0) {
        return false;
    }

    id = parsed_id;
    return true;
}

// Parse and validate a latitude or longitude.
bool parse_coordinate(
    std::string_view value,
    double minimum,
    double maximum,
    double& coordinate
) {
    if (value.empty()) {
        return false;
    }

    double parsed_value{0.0};

    const auto result = std::from_chars(
        value.data(),
        value.data() + value.size(),
        parsed_value,
        std::chars_format::general
    );

    if (result.ec != std::errc{} ||
        result.ptr != value.data() + value.size() ||
        !std::isfinite(parsed_value) ||
        parsed_value < minimum ||
        parsed_value > maximum) {
        return false;
    }

    coordinate = parsed_value;
    return true;
}

// Extract and preserve all tags from an OSM element.
bool read_tags(
    pugi::xml_node element,
    std::unordered_map<std::string, std::string>& tags
) {
    for (const pugi::xml_node tag : element.children("tag")) {
        const pugi::xml_attribute key = tag.attribute("k");
        const pugi::xml_attribute value = tag.attribute("v");

        if (!key || !value || key.value()[0] == '\0') {
            return false;
        }

        tags[key.value()] = value.value();
    }

    return true;
}

// Convert the OSM oneway tag into MapForge's
// normalized road direction.
core::RoadDirection parse_road_direction(
    const std::string& value
) {
    if (value == "yes" ||
        value == "true" ||
        value == "1") {
        return core::RoadDirection::forward;
    }

    if (value == "-1") {
        return core::RoadDirection::reverse;
    }

    if (value == "no" ||
        value == "false" ||
        value == "0") {
        return core::RoadDirection::bidirectional;
    }

    return core::RoadDirection::unknown;
}

} // namespace

OSMParseResult OSMParser::load_file(
    const std::filesystem::path& file_path
) const {

    // Create a consistent error result.
    const auto failure = [](
        OSMParseError error,
        std::string message
    ) {
        OSMParseResult result;
        result.error = error;
        result.message = std::move(message);
        return result;
    };

    // -----------------------------------------------
    // 1. Open the OSM XML file
    // -----------------------------------------------

    std::ifstream file(file_path, std::ios::binary);

    if (!file.is_open()) {
        return failure(
            OSMParseError::file_open_failed,
            "Unable to open OSM file: " + file_path.string()
        );
    }

    pugi::xml_document document;

    const pugi::xml_parse_result parse_result = document.load(file);

    if (!parse_result) {
        if (parse_result.status == pugi::status_io_error) {
            return failure(
                OSMParseError::read_failed,
                "Failed to read OSM file: " + file_path.string()
            );
        }

        return failure(
            OSMParseError::invalid_xml,
            "Invalid XML in " + file_path.string() +
            ": " + parse_result.description() +
            " at offset " + std::to_string(parse_result.offset)
        );
    }

    // -----------------------------------------------
    // 2. Validate the OSM root element
    // -----------------------------------------------

    const pugi::xml_node root = document.document_element();

    if (!root || std::string_view{root.name()} != "osm") {
        return failure(
            OSMParseError::invalid_root,
            "Invalid OSM document: expected an <osm> root element."
        );
    }

    std::vector<core::Node> nodes;
    std::vector<core::Way> ways;

    // -----------------------------------------------
    // 3. Parse OSM nodes
    // -----------------------------------------------

    for (const pugi::xml_node xml_node : root.children("node")) {

        core::Node node;

        if (!parse_osm_id(
                xml_node.attribute("id").value(),
                node.id)) {
            return failure(
                OSMParseError::malformed_data,
                "Invalid or missing ID in OSM node."
            );
        }

        if (!parse_coordinate(
                xml_node.attribute("lat").value(),
                -90.0,
                90.0,
                node.location.latitude)) {
            return failure(
                OSMParseError::malformed_data,
                "Invalid or missing latitude in OSM node " +
                std::to_string(node.id)
            );
        }

        if (!parse_coordinate(
                xml_node.attribute("lon").value(),
                -180.0,
                180.0,
                node.location.longitude)) {
            return failure(
                OSMParseError::malformed_data,
                "Invalid or missing longitude in OSM node " +
                std::to_string(node.id)
            );
        }

        if (!read_tags(xml_node, node.tags)) {
            return failure(
                OSMParseError::malformed_data,
                "Invalid tag in OSM node " +
                std::to_string(node.id)
            );
        }

        nodes.push_back(std::move(node));
    }

    // -----------------------------------------------
    // 4. Parse OSM ways
    // -----------------------------------------------

    for (const pugi::xml_node xml_way : root.children("way")) {

        core::Way way;

        if (!parse_osm_id(
                xml_way.attribute("id").value(),
                way.id)) {
            return failure(
                OSMParseError::malformed_data,
                "Invalid or missing ID in OSM way."
            );
        }

        // Extract ordered node references.
        for (const pugi::xml_node nd : xml_way.children("nd")) {

            core::OSMId reference{0};

            if (!parse_osm_id(
                    nd.attribute("ref").value(),
                    reference)) {
                return failure(
                    OSMParseError::malformed_data,
                    "Invalid or missing node reference in OSM way " +
                    std::to_string(way.id)
                );
            }

            way.node_references.push_back(reference);
        }

        // Preserve all original tags.
        if (!read_tags(xml_way, way.tags)) {
            return failure(
                OSMParseError::malformed_data,
                "Invalid tag in OSM way " +
                std::to_string(way.id)
            );
        }

        // Only retain ways with a nonempty highway tag.
        const auto highway = way.tags.find("highway");

        if (highway == way.tags.end() || highway->second.empty()) {
            continue;
        }

        // Extract normalized road metadata.
        way.metadata.highway_type = highway->second;

        const auto name = way.tags.find("name");

        if (name != way.tags.end()) {
            way.metadata.name = name->second;
        }

        const auto one_way = way.tags.find("oneway");

        if (one_way != way.tags.end()) {
            way.metadata.direction =
                parse_road_direction(one_way->second);
        }

        ways.push_back(std::move(way));
    }

    // -----------------------------------------------
    // 5. Find unresolved node references
    // -----------------------------------------------

    std::unordered_set<core::OSMId> available_node_ids;

    for (const core::Node& node : nodes) {
        available_node_ids.insert(node.id);
    }

    std::unordered_set<core::OSMId> missing_ids;

    for (const core::Way& way : ways) {
        for (const core::OSMId reference : way.node_references) {

            if (!available_node_ids.contains(reference)) {
                missing_ids.insert(reference);
            }
        }
    }

    std::vector<core::OSMId> missing_references(
        missing_ids.begin(),
        missing_ids.end()
    );

    // Make the result deterministic.
    std::sort(
        missing_references.begin(),
        missing_references.end()
    );

    // -----------------------------------------------
    // 6. Return parsed OSM data
    // -----------------------------------------------

    OSMParseResult result;

    result.nodes = std::move(nodes);
    result.ways = std::move(ways);
    result.missing_node_references = std::move(missing_references);

    result.message =
        "Loaded " + std::to_string(result.nodes.size()) +
        " nodes and " + std::to_string(result.ways.size()) +
        " highway ways.";

    return result;
}

}
