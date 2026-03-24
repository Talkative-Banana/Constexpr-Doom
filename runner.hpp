#pragma once
#include "constants.hpp"
#include "handler.hpp"
#include "parsedState.hpp"
#include <array>
#include <cstdint>
#include <string_view>

constexpr STATUS loop(State &state) {
  // holy loop batman
  while (1) {
    if (state.m_activeFunction == nullptr) {
      return STATUS::OK;
    }

    Instr _op{};
    if (state.m_instrPointer == state.m_activeFunction->m_bodyCount) {
      // We need to return from the function
      // Compiler doesn't emit an explicit return at the end of the function
      _op = Instr{"return", ""};
    } else {
      _op = state.m_activeFunction->m_body[state.m_instrPointer];
    }

    switch (_op.m_op) {
    case OP::_nop: {
      state.m_instrPointer++;
      continue;
    }
    case OP::_drop: {
      state.m_opStack.Pop();
      state.m_instrPointer++;
      break;
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
      STATUS res = HandleSelect(state);
      if (res == STATUS::ERROR) {
        throw "Select Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_br_table: {
      STATUS res = HandleBrTable(state, _op);
      if (res == STATUS::ERROR) {
        throw "Branch Table Call Handling Failed!";
        return STATUS::ERROR;
      }
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
    case OP::_i32: {
      STATUS res = HandleI<int32_t>(state, _op);
      if (res == STATUS::ERROR) {
        throw "I32 Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_i64: {
      STATUS res = HandleI<int64_t>(state, _op);
      if (res == STATUS::ERROR) {
        throw "I64 Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_f32: {
      STATUS res = HandleF<float>(state, _op);
      if (res == STATUS::ERROR) {
        throw "F32 Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_f64: {
      STATUS res = HandleF<double>(state, _op);
      if (res == STATUS::ERROR) {
        throw "F64 Call Handling Failed!";
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
    case OP::_block: {
      STATUS res = HandleBlock(state);
      if (res == STATUS::ERROR) {
        throw "BLOCK Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_br: {
      STATUS res = HandleBranch(state, _op);
      if (res == STATUS::ERROR) {
        throw "BRANCH Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_br_if: {
      STATUS res = HandleBranchIf(state, _op);
      if (res == STATUS::ERROR) {
        throw "BRANCHIF Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_end: {
      STATUS res = HandleEnd(state);
      if (res == STATUS::ERROR) {
        throw "END Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_loop: {
      STATUS res = HandleLoop(state);
      if (res == STATUS::ERROR) {
        throw "Loop Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_call_indirect: {
      STATUS res = HandleCallIndirect(state);
      if (res == STATUS::ERROR) {
        throw "CallIndirect Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    default: {
      throw "Instruction not handled";
    }
    }
  }
  return STATUS::OK;
}

inline constexpr auto ParseAndRun() {
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
  size_t functionId = 0, globalId = 0;
  for (int moduleIdx = 0; moduleIdx < m_count; moduleIdx++) {
    auto span = res.second[moduleIdx];
    std::string_view module = program.substr(span.begin, span.end - span.begin);
    for (int item = 0; item < itemCount[moduleIdx]; item++) {
      auto moduleItem = items[moduleIdx][item];
      std::string_view child =
          module.substr(moduleItem.begin, moduleItem.end - moduleItem.begin);
      int c_res = SetupModule(child, state, functionId, globalId);

      if (c_res != 0) {
        throw "Invalid child format";
      }
    }
  }

  Stack &op_stk = state.m_opStack;
  Data zdata{};

  // Parameters for added main
  zdata.m_data.emplace<int32_t>(0);
  op_stk.Push(zdata);
  op_stk.Push(zdata);

  STATUS setupCall = HandleCall(state, "$main");
  if (setupCall != STATUS::OK) {
    throw "setup call failed\n";
  }

  state.m_instrPointer = 0;
  STATUS execRes = loop(state);

  if (execRes != STATUS::OK) {
    throw "Execution Failed!";
  }

  if (state.m_opStack.m_stackPointer != state.m_opStack.m_floorPointer + 1) {
    throw "No return value from main!";
  }

  Data returnValue = state.m_opStack.Pop();

  if (std::get<int32_t>(returnValue.m_data) != 0) {
    throw "Invalid return value from main!";
  }
  return state.m_frameBuffer.m_data;
}

inline constexpr auto ParseAndRunNoCheck() {
  State state{};
  auto res = ParseProgramNoCheck(program);
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
  size_t functionId = 0, globalId = 0;
  for (int moduleIdx = 0; moduleIdx < m_count; moduleIdx++) {
    auto span = res.second[moduleIdx];
    std::string_view module = program.substr(span.begin, span.end - span.begin);
    for (int item = 0; item < itemCount[moduleIdx]; item++) {
      auto moduleItem = items[moduleIdx][item];
      std::string_view child =
          module.substr(moduleItem.begin, moduleItem.end - moduleItem.begin);
      int c_res = SetupModule(child, state, functionId, globalId);

      if (c_res != 0) {
        throw "Invalid child format";
      }
    }
  }

  Stack &op_stk = state.m_opStack;
  Data zdata{};

  // Parameters for added main
  zdata.m_data.emplace<int32_t>(0);
  op_stk.Push(zdata);
  op_stk.Push(zdata);

  STATUS setupCall = HandleCall(state, "$main");
  if (setupCall != STATUS::OK) {
    throw "setup call failed\n";
  }

  state.m_instrPointer = 0;
  STATUS execRes = loop(state);

  if (execRes != STATUS::OK) {
    throw "Execution Failed!";
  }

  if (state.m_opStack.m_stackPointer != state.m_opStack.m_floorPointer + 1) {
    throw "No return value from main!";
  }

  Data returnValue = state.m_opStack.Pop();

  if (std::get<int32_t>(returnValue.m_data) != 0) {
    throw "Invalid return value from main!";
  }
  return state.m_frameBuffer.m_data;
}

inline constexpr auto RunNoCheck() {
  State state{};
  int res = make_state(state);

  if (res != 0) {
    throw "Something went wrong while setting up state.";
  }

  Stack &op_stk = state.m_opStack;
  Data zdata{};

  // Parameters for added main
  zdata.m_data.emplace<int32_t>(0);
  op_stk.Push(zdata);
  op_stk.Push(zdata);

  STATUS setupCall = HandleCall(state, "$main");
  if (setupCall != STATUS::OK) {
    throw "setup call failed\n";
  }

  state.m_instrPointer = 0;
  STATUS execRes = loop(state);

  if (execRes != STATUS::OK) {
    throw "Execution Failed!";
  }

  if (state.m_opStack.m_stackPointer != state.m_opStack.m_floorPointer + 1) {
    throw "No return value from main!";
  }

  Data returnValue = state.m_opStack.Pop();

  if (std::get<int32_t>(returnValue.m_data) != 0) {
    throw "Invalid return value from main!";
  }
  return state.m_frameBuffer.m_data;
}