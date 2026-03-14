
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
  // Get the function pointer and return address from the stack if exists
  if (state.m_activeFunction != nullptr) {
    std::size_t hash = constexpr_hash(state.m_activeFunction->m_name);
    Instr fPtr = Instr{OP::_i64, Member::_none, OperandType::_hash, "0", hash};
    // Push function address to return to after function call
    stk.Push(fPtr);
  } else {
    // If there is no active function, we are calling the main function, so push
    // a 0 as the function pointer
    Instr fPtr = Instr{OP::_i64, Member::_none, OperandType::_hash, "0", 0};
    stk.Push(fPtr);
  }

  Instr iPtr = Instr{OP::_i64, Member::_none, OperandType::_immediate, "0",
                     state.m_instrPointer + 1};
  // Push return address to recover it after function call
  stk.Push(iPtr);
  Instr bPtr = Instr{OP::_i64, Member::_none, OperandType::_immediate, "0",
                     stk.m_basePointer};
  // Push the base ptr to recover it after function call
  stk.Push(bPtr);

  // Update the base pointer to the current stack pointer for the callee
  stk.m_basePointer = stk.m_stackPointer;

  // Push the function arguments
  // get hash of the function name and use it to get the function type and
  // argument count
  size_t hash = constexpr_hash(funcName);
  Function &f = state.m_functionTable.m_data[hash % MAXFUNCTIONS];
  uint32_t argCount = f.m_paramCount;
  uint32_t localCount = f.m_localCount;

  // TODO: Get the actual argument value from the function arguments instead of
  // pushing 0
  for (uint32_t i = 0; i < argCount; i++) {
    ParamType &param = f.m_params[i];
    if (param == ParamType::_i32) {
      stk.Push(Instr{OP::_i32, Member::_none, OperandType::_immediate, "0"});
    } else if (param == ParamType::_i64) {
      stk.Push(Instr{OP::_i64, Member::_none, OperandType::_immediate, "0"});
    } else if (param == ParamType::_f32) {
      stk.Push(Instr{OP::_f32, Member::_none, OperandType::_immediate, "0"});
    } else if (param == ParamType::_f64) {
      stk.Push(Instr{OP::_f64, Member::_none, OperandType::_immediate, "0"});
    } else if (param == ParamType::_funcref) {
      stk.Push(Instr{OP::_i32, Member::_none, OperandType::_address, "0"});
    } else if (param == ParamType::_externref) {
      stk.Push(Instr{OP::_i64, Member::_none, OperandType::_address, "0"});
    } else {
      throw "Unsupported parameter type";
    }
  }
  // Push local variables
  for (uint32_t i = 0; i < localCount; i++) {
    ParamType &local = f.m_locals[i];
    if (local == ParamType::_i32) {
      stk.Push(Instr{OP::_i32, Member::_none, OperandType::_immediate, "0"});
    } else if (local == ParamType::_i64) {
      stk.Push(Instr{OP::_i64, Member::_none, OperandType::_immediate, "0"});
    } else if (local == ParamType::_f32) {
      stk.Push(Instr{OP::_f32, Member::_none, OperandType::_immediate, "0"});
    } else if (local == ParamType::_f64) {
      stk.Push(Instr{OP::_f64, Member::_none, OperandType::_immediate, "0"});
    } else if (local == ParamType::_funcref) {
      stk.Push(Instr{OP::_i32, Member::_none, OperandType::_address, "0"});
    } else if (local == ParamType::_externref) {
      stk.Push(Instr{OP::_i64, Member::_none, OperandType::_address, "0"});
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
  if (stk.m_basePointer < instr.m_operandValue + 1) {
    throw "Invalid Address used in local.get";
  }
  uint64_t address = stk.m_basePointer - instr.m_operandValue - 1;
  if (instr.m_mem == Member::_get) {
    Instr instr = Instr{OP::_i64, Member::_none, OperandType::_immediate, "0",
                        stk.m_data[address].m_operandValue};
    stk.Push(instr);
  } else {
    Instr instr = stk.Pop();
    stk.m_data[address] = instr;
  }
  state.m_instrPointer++;
  return STATUS::OK;
}

consteval STATUS HandleGlobal(State &state, const Instr &instr) {
  Global &global = state.m_global;
  Stack &stk = state.m_stack;

  // Get and Set global variables using the operand as the key in the global
  // array
  std::string_view operand = instr.m_operand;
  size_t hash = constexpr_hash(operand);
  if (instr.m_mem == Member::_get) {
    Instr instr = global.m_data[hash % MAXGLOBALS];
    stk.Push(instr);
  } else {
    Instr instr = stk.Pop();
    global.m_data[hash % MAXGLOBALS] = instr;
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
  Stack &stk = state.m_stack;
  Global &global = state.m_global;
  if (instr.m_mem == Member::_const) {
    uint64_t value = instr.m_operandValue;
    stk.Push(
        Instr{OP::_i32, Member::_none, OperandType::_immediate, "0", value});
    state.m_instrPointer++;
    return STATUS::OK;
  } else if (instr.m_mem == Member::_add || instr.m_mem == Member::_sub ||
             instr.m_mem == Member::_mul) {
    Instr b = stk.Pop();
    Instr a = stk.Pop();
    uint64_t result;
    if (instr.m_mem == Member::_add) {
      result = a.m_operandValue + b.m_operandValue;
    } else if (instr.m_mem == Member::_sub) {
      result = a.m_operandValue - b.m_operandValue;
    } else if (instr.m_mem == Member::_mul) {
      result = a.m_operandValue * b.m_operandValue;
    } else {
      throw "Unsupported I32 operation";
    }
    stk.Push(
        Instr{OP::_i32, Member::_none, OperandType::_immediate, "0", result});
    state.m_instrPointer++;
    return STATUS::OK;
  } else if (instr.m_mem == Member::_load || instr.m_mem == Member::_store) {
    Instr val = stk.Pop();
    Instr base = stk.Pop();
    uint64_t offset = instr.m_operandValue;
    if (instr.m_mem == Member::_load) {
      if (base.m_operandValue + offset >= MAXGLOBALS) {
        throw "Invalid memory access in i32.load";
      }
      Instr instr = global.m_data[base.m_operandValue + offset];
      stk.Push(instr);
    } else {
      global.m_data[base.m_operandValue + offset] = val;
    }
    state.m_instrPointer++;
    return STATUS::OK;
  } else {
    throw "Unsupported I32 member";
  }
}