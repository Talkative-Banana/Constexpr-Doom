#include "parser.hpp"
#include "state.hpp"
#include "types.hpp"
#include <array>
#include <string_view>

struct True {
  // using type = std::integral_constant<bool, true>::type;
  static constexpr bool value = true;
};

constexpr int run(const std::array<Instr, 1> &prog) {
  for (auto _op : prog) {
    switch (_op.m_op) {
    case OP::_nop: {
      continue;
    }
    case OP::_drop: {
    }
    case OP::_select: {
      using type = SELECT<True, float, int>::type;
      return std::is_same_v<type, int>;
    }
    case OP::_unreachable: {
      return Trap::terminate;
    }
    }
  }
  return Trap::terminate;
}

// constexpr std::array<Instr, 1> prog{};

constexpr State state = SetupState();

// static_assert(state.m_functionTable.m_data[0].m_name ==
// std::string_view(""));

// constexpr int result = run(prog);

// static_assert(result == 0);
