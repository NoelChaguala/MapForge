
#include <iostream>

#include "mapforge/version.hpp"

int main() {
    std::cout << "================================\n";
    std::cout << "       " << mapforge::project_name()
              << " v" << mapforge::version() << '\n';
    std::cout << "================================\n";

    std::cout << "C++ Geospatial Processing Engine\n";
    std::cout << "Application initialized successfully.\n";

    return 0;
}
