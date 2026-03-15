
#pragma once
#include "constants.hpp"
#include "parser.hpp"
#include "state.hpp"
#include "types.hpp"
#include <array>
#include <cstdint>
#include <string_view>

constexpr std::size_t constexpr_hash(std::string_view str) {
  std::size_t hash = 14695981039346656037ull; // FNV offset basis
  for (char c : str) {
    hash ^= static_cast<std::size_t>(c);
    hash *= 1099511628211ull; // FNV prime
  }
  return hash;
}

constexpr STATUS HandleCall(State &state, const std::string_view &funcName) {
  Stack &stk = state.m_stack;
  Stack &op_stk = state.m_opStack;
  // Get the function pointer and return address from the stack if exists
  if (state.m_activeFunction != nullptr) {
    std::size_t hash = constexpr_hash(state.m_activeFunction->m_name);
    Instr fPtr = Instr{OP::_i64, Member::_none, OperandType::_hash, "0",
                       static_cast<int64_t>(hash)};
    // Push function address to return to after function call
    stk.Push(fPtr);
  } else {
    // If there is no active function, we are calling the main function, so push
    // a 0 as the function pointer
    Instr fPtr = Instr{OP::_i64, Member::_none, OperandType::_hash, "0", 0};
    stk.Push(fPtr);
  }

  Instr iPtr = Instr{OP::_i64, Member::_none, OperandType::_immediate, "0",
                     static_cast<int64_t>(state.m_instrPointer + 1)};
  // Push return address to recover it after function call
  stk.Push(iPtr);
  Instr bPtr = Instr{OP::_i64, Member::_none, OperandType::_immediate, "0",
                     static_cast<int64_t>(stk.m_basePointer)};
  // Push the base ptr to recover it after function call
  stk.Push(bPtr);

  // Update the base pointer to the current stack pointer for the callee
  stk.m_basePointer = stk.m_stackPointer;

  // get hash of the function name and use it to get the function type and
  // argument count
  size_t hash = constexpr_hash(funcName);
  Function &f = state.m_functionTable.m_data[hash % MAXFUNCTIONS];

  // Push the function arguments
  uint32_t paramCount = f.m_paramCount;
  std::array<Instr, LOCALINSTRSIZE> args{};

  if (paramCount > LOCALINSTRSIZE) {
    throw "too many args";
  }

  for (uint32_t i = paramCount; i-- > 0;) {
    args[i] = op_stk.Pop();
  }

  for (uint32_t i = 0; i < paramCount; ++i) {
    ParamType &param = f.m_params[i];
    if (param == ParamType::_i32) {
      Instr instr = args[i];
      stk.Push(Instr{OP::_i32, Member::_none, OperandType::_immediate, "0",
                     instr.m_operandValue});
    } else if (param == ParamType::_i64) {
      Instr instr = args[i];
      stk.Push(Instr{OP::_i64, Member::_none, OperandType::_immediate, "0",
                     instr.m_operandValue});
    } else if (param == ParamType::_f32) {
      Instr instr = args[i];
      stk.Push(Instr{OP::_f32, Member::_none, OperandType::_immediate, "0",
                     instr.m_operandValue});
    } else if (param == ParamType::_f64) {
      Instr instr = args[i];
      stk.Push(Instr{OP::_f64, Member::_none, OperandType::_immediate, "0",
                     instr.m_operandValue});
    } else if (param == ParamType::_funcref) {
      Instr instr = args[i];
      stk.Push(Instr{OP::_i32, Member::_none, OperandType::_address, "0",
                     instr.m_operandValue});
    } else if (param == ParamType::_externref) {
      Instr instr = args[i];
      stk.Push(Instr{OP::_i64, Member::_none, OperandType::_address, "0",
                     instr.m_operandValue});
    } else {
      throw "Unsupported parameter type";
    }
  }
  // Args are already pushed by the caller, so we just need to push space for
  // local variables
  uint32_t localCount = f.m_localCount;
  // Push local variables
  for (uint32_t i = 0; i < localCount; i++) {
    ParamType &local = f.m_locals[i];
    if (local == ParamType::_i32) {
      stk.Push(Instr{OP::_i32, Member::_none, OperandType::_immediate, "0", 0});
    } else if (local == ParamType::_i64) {
      stk.Push(Instr{OP::_i64, Member::_none, OperandType::_immediate, "0", 0});
    } else if (local == ParamType::_f32) {
      stk.Push(Instr{OP::_f32, Member::_none, OperandType::_immediate, "0", 0});
    } else if (local == ParamType::_f64) {
      stk.Push(Instr{OP::_f64, Member::_none, OperandType::_immediate, "0", 0});
    } else if (local == ParamType::_funcref) {
      stk.Push(Instr{OP::_i32, Member::_none, OperandType::_address, "0", 0});
    } else if (local == ParamType::_externref) {
      stk.Push(Instr{OP::_i64, Member::_none, OperandType::_address, "0", 0});
    } else {
      throw "Unsupported local variable type";
    }
  }

  // Update the active Function and instrunction Pointer
  state.m_activeFunction = &f;
  state.m_instrPointer = 0;
  return STATUS::OK;
}

consteval STATUS HandleLocal(State &state, const Instr &instr) {
  Stack &stk = state.m_stack;
  Stack &op_stk = state.m_opStack;

  // Get and Set local variables using the operand as the offset from the base
  if (static_cast<int64_t>(stk.m_basePointer) < instr.m_operandValue + 1) {
    throw "Invalid Address used in local.get";
  }
  uint64_t address = stk.m_basePointer - instr.m_operandValue - 1;
  if (instr.m_mem == Member::_get) {
    Instr instr = Instr{OP::_i64, Member::_none, OperandType::_immediate, "0",
                        stk.m_data[address].m_operandValue};
    op_stk.Push(instr);
  } else {
    Instr instr = op_stk.Pop();
    stk.m_data[address] = instr;
  }
  state.m_instrPointer++;
  return STATUS::OK;
}

consteval STATUS HandleGlobal(State &state, const Instr &instr) {
  Global &global = state.m_global;
  Stack &op_stk = state.m_opStack;

  // Get and Set global variables using the operand as the key in the global
  // array
  std::string_view operand = instr.m_operand;
  if (operand.empty()) {
    throw "Global instructions must have an operand";
  }

  if (operand == "$__stack_pointer") {
    if (instr.m_mem == Member::_get) {
      Instr instr = global.m_data[GLOBALSTACKPOINTERLOCATION];
      op_stk.Push(instr);
    } else {
      Instr instr = op_stk.Pop();
      global.m_data[GLOBALSTACKPOINTERLOCATION] = instr;
    }
  }
  state.m_instrPointer++;
  return STATUS::OK;
}

consteval STATUS HandleReturn(State &state) {
  // Move the stack pointer back to the base pointer
  Stack &stk = state.m_stack;
  stk.m_stackPointer = stk.m_basePointer;
  // Pop the base pointer and return address
  Instr bPtr = stk.Pop();
  Instr iPtr = stk.Pop();
  Instr fPtr = stk.Pop();
  // Update the instruction pointer and base pointer to return to the caller
  stk.m_basePointer = bPtr.m_operandValue;
  state.m_instrPointer = iPtr.m_operandValue;
  // Restore the active function
  if (fPtr.m_operandValue == 0) {
    // If the function pointer is 0, it means we are returning from the main
    // function
    state.m_activeFunction = nullptr;
    return STATUS::OK;
  }
  Function &f =
      state.m_functionTable.m_data[fPtr.m_operandValue % MAXFUNCTIONS];
  state.m_activeFunction = &f;
  return STATUS::OK;
}

consteval STATUS HandleI32(State &state, const Instr &instr) {
  Stack &op_stk = state.m_opStack;
  Global &global = state.m_global;
  if (instr.m_mem == Member::_const) {
    int64_t value = instr.m_operandValue;
    op_stk.Push(
        Instr{OP::_i32, Member::_none, OperandType::_immediate, "0", value});
    state.m_instrPointer++;
    return STATUS::OK;
  } else if (instr.m_mem == Member::_add || instr.m_mem == Member::_sub ||
             instr.m_mem == Member::_mul) {
    Instr b = op_stk.Pop();
    Instr a = op_stk.Pop();
    int64_t result;
    if (instr.m_mem == Member::_add) {
      result = a.m_operandValue + b.m_operandValue;
    } else if (instr.m_mem == Member::_sub) {
      result = a.m_operandValue - b.m_operandValue;
    } else if (instr.m_mem == Member::_mul) {
      result = a.m_operandValue * b.m_operandValue;
    } else {
      throw "Unsupported I32 operation";
    }
    op_stk.Push(
        Instr{OP::_i32, Member::_none, OperandType::_immediate, "0", result});
    state.m_instrPointer++;
    return STATUS::OK;
  } else if (instr.m_mem == Member::_load || instr.m_mem == Member::_store) {

    uint64_t offset = instr.m_operandValue;
    if (instr.m_mem == Member::_load) {
      Instr base = op_stk.Pop();
      if (base.m_operandValue + offset >= 66560) {
        throw "Invalid memory access in i32.load";
      }
      Instr instr = global.m_data[base.m_operandValue + offset];
      op_stk.Push(instr);
    } else {
      Instr val = op_stk.Pop();
      Instr base = op_stk.Pop();
      if (base.m_operandValue + offset >= 66560) {
        throw "Invalid memory access in i32.store";
      }
      global.m_data[base.m_operandValue + offset] = val;
    }
    state.m_instrPointer++;
    return STATUS::OK;
  } else {
    throw "Unsupported I32 member";
  }
}