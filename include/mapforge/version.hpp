
#pragma once

#include <string_view>

namespace mapforge {

[[nodiscard]] std::string_view version() noexcept;

[[nodiscard]] std::string_view project_name() noexcept;

}
