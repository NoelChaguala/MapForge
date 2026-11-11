
#include "mapforge/osm/osm_parser.hpp"

#include <fstream>

#include <pugixml.hpp>

namespace mapforge::osm {

OSMParseResult OSMParser::load_file(
    const std::filesystem::path& file_path
) const {

    std::ifstream file(file_path, std::ios::binary);

    if (!file.is_open()) {
        return {
            OSMParseError::file_open_failed,
            "Unable to open OSM file: " + file_path.string()
        };
    }

    pugi::xml_document document;

    const pugi::xml_parse_result parse_result = document.load(file);

    if (!parse_result) {
        if (parse_result.status == pugi::status_io_error) {
            return {
                OSMParseError::read_failed,
                "Failed to read OSM file: " + file_path.string()
            };
        }

        return {
            OSMParseError::invalid_xml,
            "Invalid XML in " + file_path.string() +
            ": " + parse_result.description() +
            " at offset " + std::to_string(parse_result.offset)
        };
    }

    const pugi::xml_node root = document.document_element();

    if (!root || std::string_view{root.name()} != "osm") {
        return {
            OSMParseError::invalid_root,
            "Invalid OSM document: expected an <osm> root element."
        };
    }

    return {
        OSMParseError::none,
        "OSM XML file loaded successfully."
    };
}

}
