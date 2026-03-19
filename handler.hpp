
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
    Data fPtr;
    fPtr.m_data = static_cast<int32_t>(hash % MAXFUNCTIONS);
    // Push function address to return to after function call
    stk.Push(fPtr);
  } else {
    // If there is no active function, we are calling the main function, so push
    // a 0 as the function pointer
    Data fPtr;
    fPtr.m_data = int32_t{0};
    stk.Push(fPtr);
  }

  Data iPtr, bPtr;
  iPtr.m_data = static_cast<int32_t>(state.m_instrPointer + 1);
  // Push return address to recover it after function call
  stk.Push(iPtr);

  bPtr.m_data = static_cast<int32_t>(stk.m_basePointer);
  // Push the base ptr to recover it after function call
  stk.Push(bPtr);

  // Update the base pointer to the current stack pointer for the callee
  stk.m_basePointer = stk.m_stackPointer;

  // get hash of the function name and use it to get the function type and
  // argument count
  size_t hash = constexpr_hash(funcName);
  int64_t h = static_cast<int64_t>(hash % MAXFUNCTIONS);
  Function &f = state.m_functionTable.m_data[h % MAXFUNCTIONS];

  // Push the function arguments
  uint32_t paramCount = f.m_paramCount;
  std::array<Data, LOCALINSTRSIZE> args{};

  if (paramCount > LOCALINSTRSIZE) {
    throw "too many args";
  }

  for (uint32_t i = paramCount; i-- > 0;) {
    args[i] = op_stk.Pop();
  }

  for (uint32_t i = 0; i < paramCount; ++i) {
    stk.Push(args[i]);
  }
  // Args are already pushed by the caller, so we just need to push space for
  // local variables
  uint32_t localCount = f.m_localCount;
  // Push local variables
  for (uint32_t i = 0; i < localCount; i++) {
    ParamType pt = f.m_params[i];
    Data data{};
    if (pt == ParamType::_i32) {
      data.m_data = int32_t{};
    } else if (pt == ParamType::_i64) {
      data.m_data = int64_t{};
    } else {
      throw "Unhandled param type";
    }
    stk.Push(data);
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
    Data data = stk.m_data[address];
    op_stk.Push(data);
  } else {
    Data data = op_stk.Pop();
    stk.m_data[address] = data;
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
      Data newData{};
      int32_t val{};
      for (size_t i = 0; i < __SIZEOF_INT__; i++) {
        val |= static_cast<int>(global.m_data[GLOBALSTACKPOINTERLOCATION + i])
               << (i * 8);
      }
      newData.m_data = int32_t{val};
      op_stk.Push(newData);
    } else {
      Data data = op_stk.Pop();
      int32_t val = std::get<int32_t>(data.m_data);
      for (size_t i = 0; i < __SIZEOF_INT__; i++) {
        global.m_data[GLOBALSTACKPOINTERLOCATION + i] =
            static_cast<uint8_t>((val >> (i * 8)) & 0xFF);
      }
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
  Data bPtr = stk.Pop();
  Data iPtr = stk.Pop();
  Data fPtr = stk.Pop();
  // Update the instruction pointer and base pointer to return to the caller
  stk.m_basePointer = std::get<int32_t>(bPtr.m_data);
  state.m_instrPointer = std::get<int32_t>(iPtr.m_data);
  // Restore the active function
  if (std::get<int32_t>(fPtr.m_data) == 0) {
    // If the function pointer is 0, it means we are returning from the main
    // function
    state.m_activeFunction = nullptr;
    return STATUS::OK;
  }

  if (std::get<int32_t>(fPtr.m_data) < 0) {
    throw "fPtr corrupted";
  }

  Function &f = state.m_functionTable
                    .m_data[std::get<int32_t>(fPtr.m_data) % MAXFUNCTIONS];
  state.m_activeFunction = &f;
  return STATUS::OK;
}

consteval STATUS HandleBlock(State &state) {
  Function *f = state.m_activeFunction;
  uint64_t instrPointer = state.m_instrPointer;

  bool blockFound = false;
  uint32_t blockId{};
  for (uint32_t blockIdx = 0; blockIdx < f->m_blockIdx; blockIdx++) {
    if (f->m_blockTable[blockIdx].m_blockStart == instrPointer) {
      blockId = blockIdx;
      blockFound = true;
      break;
    }
  }

  if (!blockFound) {
    throw "Block Idx not found";
  }

  f->Push(blockId);
  state.m_instrPointer++;
  return STATUS::OK;
}

consteval STATUS HandleBranch(State &state, Instr &instr) {
  Function *f = state.m_activeFunction;

  // Number of blocks to remove
  int blocksToRemove = instr.m_operandValue + 1;

  Block block{};
  while (blocksToRemove--) {
    block = f->Pop();
  }

  if (block.m_blockEnd == 0) {
    throw "jmp address is 0";
  }

  if (block.m_isLoop) {
    state.m_instrPointer = block.m_blockStart; // Jump to beginnig of the block
  } else {
    state.m_instrPointer = block.m_blockEnd + 1; // Jump to end of block
  }
  return STATUS::OK;
}

consteval STATUS HandleBranchIf(State &state, Instr &instr) {
  Function *f = state.m_activeFunction;

  Stack &opStack = state.m_opStack;
  Data topInstr = opStack.Pop();

  // Check if we want to take branch
  if (std::get<int32_t>(topInstr.m_data)) {

    // Number of blocks to remove
    int blocksToRemove = instr.m_operandValue + 1;

    Block block{};
    while (blocksToRemove--) {
      block = f->Pop();
    }

    if (block.m_blockEnd == 0) {
      throw "jmp address is 0";
    }
    if (block.m_isLoop) {
      state.m_instrPointer =
          block.m_blockStart; // Jump to beginnig of the block
    } else {
      state.m_instrPointer = block.m_blockEnd + 1; // Jump to end of block
    }
  } else {
    state.m_instrPointer++;
  }
  return STATUS::OK;
}

consteval STATUS HandleEnd(State &state) {
  Function *f = state.m_activeFunction;

  // Check if this end matches the block on top of the stack
  if (f->m_blockStackPointer < BLOCKSTACKSIZE) {
    uint32_t topIdx = f->m_blockStack[f->m_blockStackPointer];
    Block &top = f->m_blockTable[topIdx];

    if (top.m_blockEnd == state.m_instrPointer) {
      f->Pop(); // naturally reached end of this block
    }
  }
  state.m_instrPointer++;
  return STATUS::OK;
}

consteval STATUS HandleLoop(State &state) {
  // Loop is a special kind of block
  return HandleBlock(state);
}

consteval bool isArithmetic(const Instr &instr) {
  return instr.m_mem == Member::_add || instr.m_mem == Member::_sub ||
         instr.m_mem == Member::_mul || instr.m_mem == Member::_le_s ||
         instr.m_mem == Member::_and || instr.m_mem == Member::_eqz ||
         instr.m_mem == Member::_lt_s || instr.m_mem == Member::_rem_s ||
         instr.m_mem == Member::_shl || instr.m_mem == Member::_div_s ||
         instr.m_mem == Member::_ne || instr.m_mem == Member::_shr_s ||
         instr.m_mem == Member::_gt_s || instr.m_mem == Member::_eq ||
         instr.m_mem == Member::_xor;
}

consteval bool isSingleOperand(const Instr &instr) {
  return instr.m_mem == Member::_eqz;
}

template <typename T1>
consteval STATUS HandleI(State &state, const Instr &instr) {
  Stack &op_stk = state.m_opStack;
  Global &global = state.m_global;
  if (instr.m_mem == Member::_const) {
    Data data;
    data.m_data = static_cast<T1>(instr.m_operandValue);
    op_stk.Push(data);
    state.m_instrPointer++;
    return STATUS::OK;
  } else if (isArithmetic(instr)) {
    Data b = op_stk.Pop();
    Data a{};
    if (!isSingleOperand(instr)) {
      a.m_data = op_stk.Pop().m_data;
    }
    T1 result;
    if (instr.m_mem == Member::_add) {
      result = std::get<T1>(a.m_data) + std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_sub) {
      result = std::get<T1>(a.m_data) - std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_mul) {
      result = std::get<T1>(a.m_data) * std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_le_s) {
      result = std::get<T1>(a.m_data) <= std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_and) {
      result = std::get<T1>(a.m_data) & std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_eqz) {
      result = (std::get<T1>(b.m_data) == 0) ? 1 : 0;
    } else if (instr.m_mem == Member::_lt_s) {
      result = std::get<T1>(a.m_data) < std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_rem_s) {
      result = std::get<T1>(a.m_data) % std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_shl) {
      result = std::get<T1>(a.m_data) << std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_div_s) {
      result = std::get<T1>(a.m_data) / std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_ne) {
      result = std::get<T1>(a.m_data) != std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_shr_s) {
      result = std::get<T1>(a.m_data) >> std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_gt_s) {
      result = std::get<T1>(a.m_data) > std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_eq) {
      result = std::get<T1>(a.m_data) == std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_xor) {
      result = std::get<T1>(a.m_data) ^ std::get<T1>(b.m_data);
    } else {
      throw "Unsupported I32 operation";
    }
    Data data;
    data.m_data = static_cast<T1>(result);
    op_stk.Push(data);
    state.m_instrPointer++;
    return STATUS::OK;
  } else if ((instr.m_mem == Member::_load) ||
             (instr.m_mem == Member::_load8_u) ||
             (instr.m_mem == Member::_store)) {

    int32_t offset = instr.m_operandValue;
    if (instr.m_mem == Member::_load) {
      Data base = op_stk.Pop();
      int32_t addr = std::get<int32_t>(base.m_data);
      // Address are always 32 bit
      if (addr + offset >= 66560) {
        throw "Invalid memory access in ixx.load";
      }
      T1 val{};
      for (size_t i = 0; i < sizeof(T1); i++) {
        val |= static_cast<T1>(global.m_data[addr + offset + i]) << (i * 8);
      }

      Data data;
      data.m_data = static_cast<T1>(val);
      op_stk.Push(data);
    } else if (instr.m_mem == Member::_load8_u) {
      Data base = op_stk.Pop();
      if (std::get<T1>(base.m_data) + offset >= 66560) {
        throw "Invalid memory access in i32.load";
      }
      int8_t val{};
      val |=
          static_cast<int>(global.m_data[std::get<T1>(base.m_data) + offset]);
      Data data;
      data.m_data = static_cast<T1>(val);
      op_stk.Push(data);
    } else if (instr.m_mem == Member::_store) {
      Data val = op_stk.Pop();
      Data base = op_stk.Pop();
      int32_t addr = std::get<int32_t>(base.m_data);
      if (addr + offset >= 66560) {
        throw "Invalid memory access in ixx.store";
      }
      for (size_t i = 0; i < sizeof(T1); i++) {
        global.m_data[addr + offset + i] =
            static_cast<uint8_t>((std::get<T1>(val.m_data) >> (i * 8)) & 0xFF);
      }
    }
    state.m_instrPointer++;
    return STATUS::OK;
  } else {
    throw "Unsupported I32 member";
  }
}