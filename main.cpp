#include "parser.hpp"
#include "runner.hpp"
#include "state.hpp"
#include "types.hpp"
#include <array>
#include <string_view>

constexpr STATUS result = Run();

static_assert(result == STATUS::OK);