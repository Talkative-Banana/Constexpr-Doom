
#pragma once
#include "constants.hpp"
#include "implementation.hpp"
#include "parser.hpp"
#include "state.hpp"
#include "syscall.hpp"
#include "types.hpp"
#include <array>
#include <bit>
#include <cstdint>
#include <string_view>

constexpr STATUS HandleCall(State &state, const std::string_view &funcName) {
  Stack &stk = state.m_stack;
  Stack &op_stk = state.m_opStack;

  // get hash of the function name and use it to get the function type and
  // argument count
  size_t funcId = getFunctionId(state, funcName);
  Function &f = state.m_functionTable.m_data[funcId];

  if (!f.m_isDefined) {
    if (isSystemCall(funcName)) {
      return dispatchSysCall(state, funcName);
    } else if (isImplementationCall(funcName)) {
      return dispatchImplemCall(state, funcName);
    } else {
      throw "function definition not found";
    }
  }

  // Get the function pointer and return address from the stack if exists
  if (state.m_activeFunction != nullptr) {
    std::size_t funcId = getFunctionId(state, state.m_activeFunction->m_name);
    Data fPtr;
    fPtr.m_data = static_cast<int32_t>(funcId);
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
    } else if (pt == ParamType::_f32) {
      data.m_data = float{};
    } else if (pt == ParamType::_f64) {
      data.m_data = double{};
    } else {
      throw "Unhandled param type";
    }
    stk.Push(data);
  }

  // Update the active Function and instrunction Pointer
  state.m_activeFunction = &f;
  state.m_instrPointer = 0;

  // Clear varg count for the new function
  op_stk.m_vargCount = 0;
  return STATUS::OK;
}

constexpr STATUS HandleLocal(State &state, const Instr &instr) {
  Stack &stk = state.m_stack;
  Stack &op_stk = state.m_opStack;

  // Get and Set local variables using the operand as the offset from the base
  if (static_cast<int64_t>(stk.m_basePointer) + instr.m_operandValue + 1 >=
      STACKSIZE) {
    throw "Invalid Address used in local.get";
  }
  uint64_t address = stk.m_basePointer + instr.m_operandValue + 1;
  if (instr.m_mem == Member::_get) {
    Data data = stk.m_data[address];
    op_stk.Push(data);
  } else if (instr.m_mem == Member::_tee) {
    Data data = op_stk.Pop();
    stk.m_data[address] = data;
    op_stk.Push(data);
  } else {
    Data data = op_stk.Pop();
    stk.m_data[address] = data;
  }
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS HandleGlobal(State &state, const Instr &instr) {
  Global &global = state.m_global;
  Stack &op_stk = state.m_opStack;

  // Get and Set global variables using the operand as the key in the global
  // array
  std::string_view operand = instr.m_operand;
  if (operand.empty()) {
    throw "Global instructions must have an operand";
  }

  // get hash of the function name and use it to get the function type and
  // argument count
  size_t id = getOperandId(state, operand);
  if (instr.m_mem == Member::_get) {
    Data newData = global.m_data[id];
    op_stk.Push(newData);
  } else {
    Data data = op_stk.Pop();
    global.m_data[id] = data;
  }
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS HandleSelect(State &state) {
  Stack &op_stk = state.m_opStack;

  Data cond = op_stk.Pop();
  Data val2 = op_stk.Pop();
  Data val1 = op_stk.Pop();

  if (std::get<int32_t>(cond.m_data))
    op_stk.Push(val1);
  else
    op_stk.Push(val2);
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS HandleReturn(State &state) {
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

constexpr STATUS HandleBlock(State &state) {
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

constexpr STATUS HandleBranch(State &state, Instr &instr) {
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

constexpr STATUS HandleBrTable(State &state, const Instr &instr) {
  // pop index
  Data idxData = state.m_opStack.Pop();
  int32_t idx = std::get<int32_t>(idxData.m_data);

  // clamp to default if out of bounds
  // m_brTable is your array of branch targets

  if (idx < 0) {
    throw "Invalid negative index in br_table";
  }

  // m_operandValue is the count
  int32_t target;
  if (idx <= instr.m_brCount - 1) {
    target = instr.m_brTable[idx];
  } else {
    target = instr.m_brTable[instr.m_brCount - 1]; // default
  }

  Instr newInstr{};
  newInstr.m_op = OP::_br;
  newInstr.m_mem = Member::_none;
  newInstr.m_operandValue = target;

  return HandleBranch(state, newInstr);
}

constexpr STATUS HandleBranchIf(State &state, Instr &instr) {
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
      // Jump to beginnig of the block
      state.m_instrPointer = block.m_blockStart;
    } else {
      // Jump to end of block
      state.m_instrPointer = block.m_blockEnd + 1;
    }
  } else {
    state.m_instrPointer++;
  }
  return STATUS::OK;
}

constexpr STATUS HandleEnd(State &state) {
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

constexpr STATUS HandleLoop(State &state) {
  // Loop is a special kind of block
  return HandleBlock(state);
}

constexpr STATUS HandleCallIndirect(State &state) {
  Stack &op_stk = state.m_opStack;

  // pop table index first
  Data idxData = op_stk.Pop();
  int32_t tableIdx = std::get<int32_t>(idxData.m_data);

  // look up function in virtual table
  Function *f = state.m_virtualTable.m_data[tableIdx];
  if (f == nullptr)
    throw "call_indirect: null function pointer";

  // now call it — arguments are already on op_stk
  return HandleCall(state, f->m_name);
}

constexpr bool isArithmetic(const Instr &instr) {
  return instr.m_mem == Member::_add || instr.m_mem == Member::_sub ||
         instr.m_mem == Member::_mul || instr.m_mem == Member::_le_s ||
         instr.m_mem == Member::_ge_s || instr.m_mem == Member::_ge_u ||
         instr.m_mem == Member::_and || instr.m_mem == Member::_eqz ||
         instr.m_mem == Member::_lt_s || instr.m_mem == Member::_lt_u ||
         instr.m_mem == Member::_rem_s || instr.m_mem == Member::_rem_u ||
         instr.m_mem == Member::_shl || instr.m_mem == Member::_div_u ||
         instr.m_mem == Member::_div_s || instr.m_mem == Member::_ne ||
         instr.m_mem == Member::_shr_u || instr.m_mem == Member::_shr_s ||
         instr.m_mem == Member::_gt_s || instr.m_mem == Member::_gt_u ||
         instr.m_mem == Member::_eq || instr.m_mem == Member::_xor ||
         instr.m_mem == Member::_or || instr.m_mem == Member::_trunc_f32_s ||
         instr.m_mem == Member::_trunc_f64_s ||
         instr.m_mem == Member::_extend_i32_s ||
         instr.m_mem == Member::_wrap_i64;
}

constexpr bool isArithmeticDecimal(const Instr &instr) {
  return instr.m_mem == Member::_add || instr.m_mem == Member::_sub ||
         instr.m_mem == Member::_div || instr.m_mem == Member::_ge ||
         instr.m_mem == Member::_mul || instr.m_mem == Member::_eqz ||
         instr.m_mem == Member::_ne || instr.m_mem == Member::_eq ||
         instr.m_mem == Member::_abs || instr.m_mem == Member::_lt ||
         instr.m_mem == Member::_convert_i32_s ||
         instr.m_mem == Member::_convert_i64_s ||
         instr.m_mem == Member::_promote_f32 ||
         instr.m_mem == Member::_demote_f64;
}

constexpr bool isSingleOperand(const Instr &instr) {
  return instr.m_mem == Member::_eqz || instr.m_mem == Member::_abs ||
         instr.m_mem == Member::_trunc_f32_s ||
         instr.m_mem == Member::_trunc_f64_s ||
         instr.m_mem == Member::_convert_i32_s ||
         instr.m_mem == Member::_convert_i64_s ||
         instr.m_mem == Member::_promote_f32 ||
         instr.m_mem == Member::_demote_f64 ||
         instr.m_mem == Member::_extend_i32_s ||
         instr.m_mem == Member::_wrap_i64;
}

template <typename T1>
constexpr STATUS HandleI(State &state, const Instr &instr) {
  Stack &op_stk = state.m_opStack;
  Memory &memory = state.m_memory;
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
    } else if (instr.m_mem == Member::_lt_u) {
      using UT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      result = static_cast<UT>(std::get<T1>(a.m_data)) <
               static_cast<UT>(std::get<T1>(b.m_data));
    } else if (instr.m_mem == Member::_rem_s) {
      if (std::get<T1>(b.m_data) == 0) {
        throw "rem_s: division by zero";
      }
      result = std::get<T1>(a.m_data) % std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_rem_u) {
      using UT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      UT ua = static_cast<UT>(std::get<T1>(a.m_data));
      UT ub = static_cast<UT>(std::get<T1>(b.m_data));
      if (ub == 0) {
        throw "rem_u: division by zero";
      }
      result = static_cast<T1>(ua % ub);
    } else if (instr.m_mem == Member::_shl) {
      result = std::get<T1>(a.m_data) << std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_div_s) {
      result = std::get<T1>(a.m_data) / std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_div_u) {
      using UT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      result = static_cast<T1>(static_cast<UT>(std::get<T1>(a.m_data)) /
                               static_cast<UT>(std::get<T1>(b.m_data)));
    } else if (instr.m_mem == Member::_ne) {
      result = std::get<T1>(a.m_data) != std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_shr_s) {
      result = std::get<T1>(a.m_data) >> std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_shr_u) {
      using UT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      result = static_cast<T1>(static_cast<UT>(std::get<T1>(a.m_data)) >>
                               static_cast<UT>(std::get<T1>(b.m_data)));
    } else if (instr.m_mem == Member::_gt_s) {
      result = std::get<T1>(a.m_data) > std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_ge_s) {
      result = std::get<T1>(a.m_data) >= std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_ge_u) {
      using UT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      result = static_cast<UT>(std::get<T1>(a.m_data)) >=
               static_cast<UT>(std::get<T1>(b.m_data));
    } else if (instr.m_mem == Member::_gt_u) {
      using UT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      result = static_cast<UT>(std::get<T1>(a.m_data)) >
               static_cast<UT>(std::get<T1>(b.m_data));
    } else if (instr.m_mem == Member::_eq) {
      result = std::get<T1>(a.m_data) == std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_xor) {
      result = std::get<T1>(a.m_data) ^ std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_or) {
      result = std::get<T1>(a.m_data) | std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_trunc_f32_s) {
      result = static_cast<T1>(std::get<float>(b.m_data));
    } else if (instr.m_mem == Member::_trunc_f64_s) {
      result = static_cast<T1>(std::get<double>(b.m_data));
    } else if (instr.m_mem == Member::_extend_i32_s) {
      result = static_cast<T1>(std::get<int32_t>(b.m_data));
    } else if (instr.m_mem == Member::_wrap_i64) {
      result = static_cast<int32_t>(std::get<int64_t>(b.m_data));
    } else {
      throw "Unsupported Ixx operation";
    }
    Data data;
    data.m_data = static_cast<T1>(result);
    op_stk.Push(data);
    state.m_instrPointer++;
    return STATUS::OK;
  } else if ((instr.m_mem == Member::_load) ||
             (instr.m_mem == Member::_load8_u) ||
             (instr.m_mem == Member::_load8_s) ||
             (instr.m_mem == Member::_load16_u) ||
             (instr.m_mem == Member::_load16_s) ||
             (instr.m_mem == Member::_store8) ||
             (instr.m_mem == Member::_store16) ||
             (instr.m_mem == Member::_store)) {

    int32_t offset = instr.m_operandValue;
    if (instr.m_mem == Member::_load) {
      Data base = op_stk.Pop();
      int32_t addr = std::get<int32_t>(base.m_data);
      // Address are always 32 bit
      if (addr < 0) {
        throw "Invalid base ptr in load";
      }
      if (offset < 0) {
        throw "Invalid offset in load";
      }
      if (addr + offset >= MEMORYSIZE) {
        throw "Invalid memory access in ixx.load";
      }
      T1 val{};
      for (size_t i = 0; i < sizeof(T1); i++) {
        val |= static_cast<T1>(memory.m_data[addr + offset + i]) << (i * 8);
      }

      Data data;
      data.m_data = static_cast<T1>(val);
      op_stk.Push(data);
    } else if (instr.m_mem == Member::_load8_u) {
      Data base = op_stk.Pop();
      int32_t addr = std::get<int32_t>(base.m_data);
      if (addr < 0) {
        throw "Invalid base ptr in load8_u";
      }
      if (offset < 0) {
        throw "Invalid offset in load8_u";
      }
      if (addr + offset >= MEMORYSIZE) {
        throw "Invalid memory access in i32.load";
      }
      uint8_t val = memory.m_data[addr + offset];
      Data data;
      data.m_data = static_cast<T1>(val);
      op_stk.Push(data);
    } else if (instr.m_mem == Member::_load8_s) {
      Data base = op_stk.Pop();
      int32_t addr = std::get<int32_t>(base.m_data);
      if (addr < 0) {
        throw "Invalid base ptr in load8_s";
      }

      if (offset < 0) {
        throw "Invalid offset in load8_s";
      }
      if (addr + offset >= MEMORYSIZE) {
        throw "Invalid memory access in i32.load";
      }
      int8_t val = static_cast<int8_t>(memory.m_data[addr + offset]);
      Data data;
      data.m_data = static_cast<T1>(val);
      op_stk.Push(data);
    } else if (instr.m_mem == Member::_load16_u) {
      Data base = op_stk.Pop();
      int32_t addr = std::get<int32_t>(base.m_data);
      if (addr < 0) {
        throw "Invalid base ptr in load16_u";
      }

      if (offset < 0) {
        throw "Invalid offset in load16_u";
      }
      if (addr + offset >= MEMORYSIZE) {
        throw "Invalid memory access in ixx.load16_u";
      }
      uint16_t val{};
      for (size_t i = 0; i < sizeof(uint16_t); i++) {
        val |= static_cast<uint16_t>(memory.m_data[addr + offset + i])
               << (i * 8);
      }
      Data data;
      data.m_data = static_cast<T1>(val);
      op_stk.Push(data);
    } else if (instr.m_mem == Member::_load16_s) {
      Data base = op_stk.Pop();
      int32_t addr = std::get<int32_t>(base.m_data);
      if (addr < 0) {
        throw "Invalid base ptr in load16_s";
      }

      if (offset < 0) {
        throw "Invalid offset in load16_s";
      }
      if (addr + offset >= MEMORYSIZE) {
        throw "Invalid memory access in i32.load";
      }
      int16_t val{};
      for (size_t i = 0; i < sizeof(int16_t); i++) {
        val |= static_cast<int16_t>(memory.m_data[addr + offset + i])
               << (i * 8);
      }
      Data data;
      data.m_data = static_cast<T1>(val);
      op_stk.Push(data);
    } else if (instr.m_mem == Member::_store ||
               instr.m_mem == Member::_store8 ||
               instr.m_mem == Member::_store16) {
      Data val = op_stk.Pop();
      Data base = op_stk.Pop();
      int32_t addr = std::get<int32_t>(base.m_data);
      if (addr < 0) {
        throw "Invalid base ptr in store/store8/store16";
      }

      if (offset < 0) {
        throw "Invalid offset in store/store8/store16";
      }

      if (addr + offset >= MEMORYSIZE) {
        throw "Invalid memory access in ixx.store";
      }

      size_t bytes;
      if (instr.m_mem == Member::_store8) {
        bytes = 1;
      } else if (instr.m_mem == Member::_store16) {
        bytes = 2;
      } else {
        bytes = sizeof(T1);
      }

      for (size_t i = 0; i < bytes; i++) {
        memory.m_data[addr + offset + i] =
            static_cast<uint8_t>((std::get<T1>(val.m_data) >> (i * 8)) & 0xFF);
      }
    }
    state.m_instrPointer++;
    return STATUS::OK;
  } else {
    throw "Unsupported I member";
  }
}

template <typename T1>
constexpr STATUS HandleF(State &state, const Instr &instr) {
  Stack &op_stk = state.m_opStack;
  Memory &memory = state.m_memory;
  if (instr.m_mem == Member::_const) {
    Data data;
    data.m_data = static_cast<T1>(instr.m_operandValueDecimal);
    op_stk.Push(data);
    state.m_instrPointer++;
    return STATUS::OK;
  } else if (isArithmeticDecimal(instr)) {
    Data b = op_stk.Pop();
    Data a{};
    if (!isSingleOperand(instr)) {
      a.m_data = op_stk.Pop().m_data;
    }
    T1 result;
    bool isAssert = false;
    if (instr.m_mem == Member::_add) {
      result = std::get<T1>(a.m_data) + std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_sub) {
      result = std::get<T1>(a.m_data) - std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_mul) {
      result = std::get<T1>(a.m_data) * std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_div) {
      result = std::get<T1>(a.m_data) / std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_eqz) {
      result = (std::get<T1>(b.m_data) == 0) ? 1 : 0;
      isAssert = true;
    } else if (instr.m_mem == Member::_ne) {
      result = std::get<T1>(a.m_data) != std::get<T1>(b.m_data);
      isAssert = true;
    } else if (instr.m_mem == Member::_eq) {
      result = std::get<T1>(a.m_data) == std::get<T1>(b.m_data);
      isAssert = true;
    } else if (instr.m_mem == Member::_abs) {
      result = std::get<T1>(b.m_data) >= 0 ? std::get<T1>(b.m_data)
                                           : -std::get<T1>(b.m_data);
    } else if (instr.m_mem == Member::_lt) {
      result = std::get<T1>(a.m_data) < std::get<T1>(b.m_data);
      isAssert = true;
    } else if (instr.m_mem == Member::_ge) {
      result = std::get<T1>(a.m_data) >= std::get<T1>(b.m_data);
      isAssert = true;
    } else if (instr.m_mem == Member::_convert_i32_s) {
      result = static_cast<T1>(std::get<int32_t>(b.m_data));
    } else if (instr.m_mem == Member::_convert_i64_s) {
      result = static_cast<T1>(std::get<int64_t>(b.m_data));
    } else if (instr.m_mem == Member::_promote_f32) {
      result = static_cast<T1>(std::get<float>(b.m_data));
    } else if (instr.m_mem == Member::_demote_f64) {
      result = static_cast<T1>(std::get<double>(b.m_data));
    } else {
      throw "Unsupported F32 operation";
    }
    Data data;
    if (isAssert) {
      data.m_data = static_cast<int32_t>(result);
    } else {
      data.m_data = static_cast<T1>(result);
    }
    op_stk.Push(data);
    state.m_instrPointer++;
    return STATUS::OK;
  } else if ((instr.m_mem == Member::_load) ||
             (instr.m_mem == Member::_store)) {

    int32_t offset = instr.m_operandValue;
    if (instr.m_mem == Member::_load) {
      Data base = op_stk.Pop();
      int32_t addr = std::get<int32_t>(base.m_data);
      // Address are always 32 bit
      if (addr < 0) {
        throw "Invalid base ptr in load F";
      }

      if (offset < 0) {
        throw "Invalid offset in load F";
      }
      if (addr + offset >= MEMORYSIZE) {
        throw "Invalid memory access in fxx.load";
      }

      using RawT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      RawT raw{};
      for (size_t i = 0; i < sizeof(T1); i++)
        raw |= static_cast<RawT>(memory.m_data[addr + offset + i]) << (i * 8);

      T1 val = std::bit_cast<T1>(raw); // constexpr safe

      Data data;
      data.m_data = static_cast<T1>(val);
      op_stk.Push(data);
    } else if (instr.m_mem == Member::_store) {
      Data val = op_stk.Pop();
      Data base = op_stk.Pop();
      int32_t addr = std::get<int32_t>(base.m_data);
      if (addr < 0) {
        throw "Invalid base ptr in store F";
      }

      if (offset < 0) {
        throw "Invalid offset in store F";
      }
      if (addr + offset >= MEMORYSIZE) {
        throw "Invalid memory access in fxx.store";
      }

      using RawT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      T1 f = std::get<T1>(val.m_data);
      RawT raw = std::bit_cast<RawT>(f); // constexpr safe

      for (size_t i = 0; i < sizeof(T1); i++) {
        memory.m_data[addr + offset + i] =
            static_cast<uint8_t>((raw >> (i * 8)) & 0xFF);
      }
    }
    state.m_instrPointer++;
    return STATUS::OK;
  } else {
    throw "Unsupported F member";
  }
}
