#pragma once
#include "constants.hpp"
#include "handler.hpp"
#include <array>
#include <cstdint>
#include <string_view>

consteval STATUS loop(State &state) {
  // holy loop batman
  while (1) {
    if (state.m_activeFunction == nullptr) {
      return STATUS::OK;
    }
    Instr _op = state.m_activeFunction->m_body[state.m_instrPointer];
    switch (_op.m_op) {
    case OP::_nop: {
      continue;
    }
    case OP::_drop: {
    }
    case OP::_local: {
      STATUS res = HandleLocal(state, _op);
      if (res == STATUS::ERROR) {
        throw "LOCAL Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_global: {
      STATUS res = HandleGlobal(state, _op);
      if (res == STATUS::ERROR) {
        throw "GLOBAL Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_select: {
      // using type = SELECT<True, float, int>::type;
      // return std::is_same_v<type, int>;
      break;
    }
    case OP::_call: {
      STATUS res = HandleCall(state, _op.m_operand);
      if (res == STATUS::ERROR) {
        throw "CALL Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_return: {
      STATUS res = HandleReturn(state);
      if (res == STATUS::ERROR) {
        throw "RETURN Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_unreachable: {
      throw "CPU Halt!";
      return STATUS::ERROR;
    }
    default: {
      state.m_instrPointer++;
      break;
    }
    }
  }
  return STATUS::OK;
}

inline consteval STATUS Run() {
  State state{};
  auto res = ParseProgram(program);
  int m_count = res.first;

  if (m_count == 0)
    throw "No module found";

  if (m_count > MAXMODULES)
    throw "Too many modules found";

  // max MAXCHILDREN module children and MAXMODULES modules
  // Validate Modules
  std::array<std::array<Span, MAXCHILDREN>, MAXMODULES> items{};
  std::array<int, MAXMODULES> itemCount{};
  for (int moduleIdx = 0; moduleIdx < m_count; moduleIdx++) {
    Span span = res.second[moduleIdx];
    std::string_view module = program.substr(span.begin, span.end - span.begin);
    uint32_t c_count = ParseModule(module, items[moduleIdx]);

    if (c_count > MAXCHILDREN) {
      throw "Too many children found";
    }
    itemCount[moduleIdx] = c_count;
  }

  // Setup Modules
  for (int moduleIdx = 0; moduleIdx < m_count; moduleIdx++) {
    auto span = res.second[moduleIdx];
    std::string_view module = program.substr(span.begin, span.end - span.begin);
    for (int item = 0; item < itemCount[moduleIdx]; item++) {
      auto moduleItem = items[moduleIdx][item];
      std::string_view child =
          module.substr(moduleItem.begin, moduleItem.end - moduleItem.begin);
      int c_res = SetupModule(child, state);

      if (c_res != 0) {
        throw "Invalid child format";
      }
    }
  }

  // End of memory (64KB) - STACKSIZE (8KB)
  // TODO: Remove hard coded initalisaiton
  Stack &stk = state.m_stack;
  stk.m_stackPointer = 66560 - 1;
  stk.m_basePointer = 66560 - 1;

  STATUS setupCall = HandleCall(state, "$main");
  if (setupCall != STATUS::OK) {
    throw "setup call failed\n";
  }

  state.m_instrPointer = 0;
  return loop(state);
}