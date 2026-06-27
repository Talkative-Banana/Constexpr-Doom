
#pragma once
#include "constants.hpp"
#include "implementation.hpp"
#include "parser.hpp"
#include "state.hpp"
#include "syscall.hpp"
#include "types.hpp"
#include <array>
#include <bit>
#include <iostream>
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
    fPtr.set(static_cast<int32_t>(funcId));
    // Push function address to return to after function call
    stk.Push(fPtr);
  } else {
    // If there is no active function, we are calling the main function, so push
    // a 0 as the function pointer
    Data fPtr;
    fPtr.set(int32_t{0});
    stk.Push(fPtr);
  }

  Data iPtr, bPtr, blockSP;

  if (state.m_activeFunction != nullptr) {
    blockSP.set(static_cast<int32_t>(state.m_activeFunction->m_blockStackPointer));
  } else {
    blockSP.set(static_cast<int32_t>(BLOCKSTACKSIZE)); // no caller, push sentinel
  }

  iPtr.set(static_cast<int32_t>(state.m_instrPointer + 1));
  // Push return address to recover it after function call
  stk.Push(iPtr);

  stk.Push(blockSP);
  // Reset block stack for new call
  f.m_blockStackPointer = BLOCKSTACKSIZE;

  bPtr.set(static_cast<int32_t>(stk.m_basePointer));
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
    auto tmp = op_stk.Pop();
    args[i] = tmp;
  }

  for (uint32_t i = 0; i < paramCount; ++i) {
    stk.Push(args[i]);
  }

//   #ifdef RUNTIME_MODE
//     if(funcName == "$STlib_drawNum"){
//     printf("\n=== STlib_drawNum ===\n");
//     printf("paramCount=%u\n", f.m_paramCount);
//     printf("localCount=%u\n", f.m_localCount);

//     for(uint32_t i=0;i<f.m_paramCount;i++)
//     {
//         auto &a = stk.m_data[stk.m_basePointer + 1 + i];

//         printf("param[%u] data=%llu str='%.*s'\n",
//                i,
//                (unsigned long long)a.m_data,
//                (int)a.m_strId.size(),
//                a.m_strId.data());
//     }
//   }
//   #endif

//   #ifdef RUNTIME_MODE
//   if (funcName == "$V_CopyRect"){
//     auto bp = stk.m_basePointer;
//     std::cerr
//         << "CopyRect("
//         << stk.m_data[bp+1].get<int32_t>() << ", "
//         << stk.m_data[bp+2].get<int32_t>() << ", "
//         << stk.m_data[bp+3].get<int32_t>() << ", "
//         << stk.m_data[bp+4].get<int32_t>() << ", "
//         << stk.m_data[bp+5].get<int32_t>() << ", "
//         << stk.m_data[bp+6].get<int32_t>() << ", "
//         << stk.m_data[bp+7].get<int32_t>() << ", "
//         << stk.m_data[bp+8].get<int32_t>()
//         << ")\n";
//   }
//   #endif

//   #ifdef RUNTIME_MODE
//     if (funcName == "$STlib_updateNum"){
//         auto bp = stk.m_basePointer;

//         uint32_t n = stk.m_data[bp + 1].get<uint32_t>();

//         std::cerr << "\nSTlib_updateNum(" << n << ")\n";

//         auto &mem = state.m_memory.m_data;

//         auto load32 = [&](uint32_t addr) -> uint32_t {
//             return mem[addr]
//                 | (mem[addr+1] << 8)
//                 | (mem[addr+2] << 16)
//                 | (mem[addr+3] << 24);
//         };

//         for(int i=0;i<48;i+=4)
//         {
//             std::cerr
//                 << "  +" << i
//                 << " = " << load32(n+i)
//                 << "\n";
//         }
// }
// #endif
  // Args are already pushed by the caller, so we just need to push space for
  // local variables
  uint32_t localCount = f.m_localCount;
  // Push local variables
  for (uint32_t i = 0; i < localCount; i++) {
    ParamType pt = f.m_locals[i];
    Data data{};
    if (pt == ParamType::_i32) {
      data.set(int32_t{});
    } else if (pt == ParamType::_i64) {
      data.set(int64_t{});
    } else if (pt == ParamType::_f32) {
      data.set(float{});
    } else if (pt == ParamType::_f64) {
      data.set(double{});
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

  int64_t address = static_cast<int64_t>(stk.m_basePointer) + static_cast<int64_t>(instr.m_operandValue) + 1;

  if (address < 0 || address >= static_cast<int64_t>(STACKSIZE)){
    throw "Invalid Address used in local.get";
  }
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

  if (cond.get<int32_t>())
    op_stk.Push(val1);
  else
    op_stk.Push(val2);
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS HandleReturn(State &state) {
  Stack &stk = state.m_stack;
  stk.m_stackPointer = stk.m_basePointer;

  Data bPtr    = stk.Pop();
  Data blockSP = stk.Pop();
  Data iPtr    = stk.Pop();
  Data fPtr    = stk.Pop();

  stk.m_basePointer    = bPtr.get<int32_t>();
  state.m_instrPointer = iPtr.get<int32_t>();

  if (fPtr.get<int32_t>() == 0) {
    state.m_activeFunction = nullptr;
    return STATUS::OK;
  }

  if (fPtr.get<int32_t>() < 0) {
    throw "fPtr corrupted";
  }

  Function &callerF = state.m_functionTable.m_data[
    fPtr.get<int32_t>() % MAXFUNCTIONS];
  callerF.m_blockStackPointer = 
    static_cast<uint32_t>(blockSP.get<int32_t>());
  state.m_activeFunction = &callerF;

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

  // Only push if top of block stack isn't already this block
  if (f->m_blockStackPointer >= BLOCKSTACKSIZE ||
      f->m_blockStack[f->m_blockStackPointer] != blockId) {
    f->Push(blockId);
  }

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
  Data idxData = state.m_opStack.Pop();
  uint32_t idx = static_cast<uint32_t>(idxData.get<int32_t>());

  uint32_t clampedIdx = (idx <= static_cast<uint32_t>(instr.m_brCount - 1)) ? idx : static_cast<uint32_t>(instr.m_brCount - 1);
  int32_t target = state.m_brTablePool.m_data[instr.m_brTableOffset + clampedIdx];

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
  if (topInstr.get<int32_t>()) {

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
  int32_t tableIdx = idxData.get<int32_t>();

  // look up function in virtual table
  Function *f = state.m_virtualTable.m_data[tableIdx];
  if (f == nullptr)
    throw "call_indirect: null function pointer";

  // now call it — arguments are already on op_stk
  return HandleCall(state, f->m_name);
}

constexpr bool isArithmetic(const Instr &instr) {
  return instr.m_mem == Member::_add || instr.m_mem == Member::_sub ||
         instr.m_mem == Member::_mul || instr.m_mem == Member::_le_s ||  instr.m_mem == Member::_le_u ||
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
    data.set(static_cast<T1>(instr.m_operandValue));
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
    // signed overflow is ub but well defined in wasm
    if (instr.m_mem == Member::_add) {
      using UT = std::make_unsigned_t<T1>;
      result = static_cast<T1>(static_cast<UT>(a.get<T1>()) + static_cast<UT>(b.get<T1>()));
    } else if (instr.m_mem == Member::_sub) {
      using UT = std::make_unsigned_t<T1>;
      result = static_cast<T1>(static_cast<UT>(a.get<T1>()) - static_cast<UT>(b.get<T1>()));
    } else if (instr.m_mem == Member::_mul) {
      using UT = std::make_unsigned_t<T1>;
      result = static_cast<T1>(static_cast<UT>(a.get<T1>()) * static_cast<UT>(b.get<T1>()));
    } else if (instr.m_mem == Member::_le_s) {
      result = a.get<T1>() <= b.get<T1>();
    } else if (instr.m_mem == Member::_le_u) {
      using UT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      result = static_cast<UT>(a.get<T1>()) <= static_cast<UT>(b.get<T1>());
    } else if (instr.m_mem == Member::_and) {
      result = a.get<T1>() & b.get<T1>();
    } else if (instr.m_mem == Member::_eqz) {
      result = (b.get<T1>() == 0) ? 1 : 0;
    } else if (instr.m_mem == Member::_lt_s) {
      result = a.get<T1>() < b.get<T1>();
    } else if (instr.m_mem == Member::_lt_u) {
      using UT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      result = static_cast<UT>(a.get<T1>()) <
               static_cast<UT>(b.get<T1>());
    } else if (instr.m_mem == Member::_rem_s) {
      if (b.get<T1>() == 0) {
        throw "rem_s: division by zero";
      }
      result = a.get<T1>() % b.get<T1>();
    } else if (instr.m_mem == Member::_rem_u) {
      using UT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      UT ua = static_cast<UT>(a.get<T1>());
      UT ub = static_cast<UT>(b.get<T1>());
      if (ub == 0) {
        throw "rem_u: division by zero";
      }
      result = static_cast<T1>(ua % ub);
    } else if (instr.m_mem == Member::_shl) {
      using UT = std::make_unsigned_t<T1>;
      result = static_cast<T1>(static_cast<UT>(a.get<T1>()) << (b.get<T1>() & (sizeof(T1) * 8 - 1)));
    } else if (instr.m_mem == Member::_shr_s) {
      result = a.get<T1>() >> (b.get<T1>() & (sizeof(T1) * 8 - 1));
    } else if (instr.m_mem == Member::_shr_u) {
      using UT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      result = static_cast<T1>(static_cast<UT>(a.get<T1>()) >> 
             (static_cast<UT>(b.get<T1>()) & (sizeof(T1) * 8 - 1)));
    } else if (instr.m_mem == Member::_div_s) {
      if (b.get<T1>() == 0) {
        throw "div_s: division by zero";
      }
      if (a.get<T1>() == std::numeric_limits<T1>::min() && b.get<T1>() == -1) {
        throw "div_s: overflow";
      }
      result = a.get<T1>() / b.get<T1>();
    } else if (instr.m_mem == Member::_div_u) {
      using UT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      result = static_cast<T1>(static_cast<UT>(a.get<T1>()) /
                               static_cast<UT>(b.get<T1>()));
    } else if (instr.m_mem == Member::_ne) {
      result = a.get<T1>() != b.get<T1>();
    } else if (instr.m_mem == Member::_gt_s) {
      result = a.get<T1>() > b.get<T1>();
    } else if (instr.m_mem == Member::_ge_s) {
      result = a.get<T1>() >= b.get<T1>();
    } else if (instr.m_mem == Member::_ge_u) {
      using UT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      result = static_cast<UT>(a.get<T1>()) >=
               static_cast<UT>(b.get<T1>());
    } else if (instr.m_mem == Member::_gt_u) {
      using UT = std::conditional_t<sizeof(T1) == 4, uint32_t, uint64_t>;
      result = static_cast<UT>(a.get<T1>()) >
               static_cast<UT>(b.get<T1>());
    } else if (instr.m_mem == Member::_eq) {
      result = a.get<T1>() == b.get<T1>();
    } else if (instr.m_mem == Member::_xor) {
      result = a.get<T1>() ^ b.get<T1>();
    } else if (instr.m_mem == Member::_or) {
      result = a.get<T1>() | b.get<T1>();
    } else if (instr.m_mem == Member::_trunc_f32_s) {
      result = static_cast<T1>(b.get<float>());
    } else if (instr.m_mem == Member::_trunc_f64_s) {
      result = static_cast<T1>(b.get<double>());
    } else if (instr.m_mem == Member::_extend_i32_s) {
      result = static_cast<T1>(b.get<int32_t>());
    } else if (instr.m_mem == Member::_wrap_i64) {
      result = static_cast<int32_t>(b.get<int64_t>());
    } else {
      throw "Unsupported Ixx operation";
    }
    Data data;
    data.set(static_cast<T1>(result));
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
      int32_t addr = base.get<int32_t>();
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
        using UT = std::make_unsigned_t<T1>;
        val |= static_cast<T1>(static_cast<UT>(memory.m_data[addr + offset + i]) << (i * 8));
      }

      Data data;
      data.set(static_cast<T1>(val));
      op_stk.Push(data);
    } else if (instr.m_mem == Member::_load8_u) {
      Data base = op_stk.Pop();
      int32_t addr = base.get<int32_t>();
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
      data.set(static_cast<T1>(val));
      op_stk.Push(data);
    } else if (instr.m_mem == Member::_load8_s) {
      Data base = op_stk.Pop();
      int32_t addr = base.get<int32_t>();
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
      data.set(static_cast<T1>(val));
      op_stk.Push(data);
    } else if (instr.m_mem == Member::_load16_u) {
      Data base = op_stk.Pop();
      int32_t addr = base.get<int32_t>();
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
      data.set(static_cast<T1>(val));
      op_stk.Push(data);
    } else if (instr.m_mem == Member::_load16_s) {
        Data base = op_stk.Pop();
        int32_t addr = base.get<int32_t>();
        if (addr < 0) throw "Invalid base ptr in load16_s";
        if (offset < 0) throw "Invalid offset in load16_s";
        if (addr + offset >= MEMORYSIZE) throw "Invalid memory access in i32.load";

        uint16_t raw = 0;
        for (size_t i = 0; i < sizeof(uint16_t); i++) {
            raw |= static_cast<uint16_t>(
                static_cast<uint16_t>(memory.m_data[addr + offset + i]) << (i * 8)
            );
        }
        // Sign-extend: reinterpret as signed int16, then extend to T1
        int16_t val = static_cast<int16_t>(raw);
        Data data;
        data.set(static_cast<T1>(val));
        op_stk.Push(data);
    } else if (instr.m_mem == Member::_store ||
               instr.m_mem == Member::_store8 ||
               instr.m_mem == Member::_store16) {
      Data val = op_stk.Pop();
      Data base = op_stk.Pop();
      int32_t addr = base.get<int32_t>();
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
            static_cast<uint8_t>((val.get<T1>() >> (i * 8)) & 0xFF);
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
    data.set(static_cast<T1>(instr.m_operandValueDecimal));
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
      result = a.get<T1>() + b.get<T1>();
    } else if (instr.m_mem == Member::_sub) {
      result = a.get<T1>() - b.get<T1>();
    } else if (instr.m_mem == Member::_mul) {
      result = a.get<T1>() * b.get<T1>();
    } else if (instr.m_mem == Member::_div) {
      result = a.get<T1>() / b.get<T1>();
    } else if (instr.m_mem == Member::_eqz) {
      result = (b.get<T1>() == 0) ? 1 : 0;
      isAssert = true;
    } else if (instr.m_mem == Member::_ne) {
      result = a.get<T1>() != b.get<T1>();
      isAssert = true;
    } else if (instr.m_mem == Member::_eq) {
      result = a.get<T1>() == b.get<T1>();
      isAssert = true;
    } else if (instr.m_mem == Member::_abs) {
      result = b.get<T1>() >= 0 ? b.get<T1>()
                                           : -b.get<T1>();
    } else if (instr.m_mem == Member::_lt) {
      result = a.get<T1>() < b.get<T1>();
      isAssert = true;
    } else if (instr.m_mem == Member::_ge) {
      result = a.get<T1>() >= b.get<T1>();
      isAssert = true;
    } else if (instr.m_mem == Member::_convert_i32_s) {
      result = static_cast<T1>(b.get<int32_t>());
    } else if (instr.m_mem == Member::_convert_i64_s) {
      result = static_cast<T1>(b.get<int64_t>());
    } else if (instr.m_mem == Member::_promote_f32) {
      result = static_cast<T1>(b.get<float>());
    } else if (instr.m_mem == Member::_demote_f64) {
      result = static_cast<T1>(b.get<double>());
    } else {
      throw "Unsupported F32 operation";
    }
    Data data;
    if (isAssert) {
      data.set(static_cast<int32_t>(result));
    } else {
      data.set(static_cast<T1>(result));
    }
    op_stk.Push(data);
    state.m_instrPointer++;
    return STATUS::OK;
  } else if ((instr.m_mem == Member::_load) ||
             (instr.m_mem == Member::_store)) {

    int32_t offset = instr.m_operandValue;
    if (instr.m_mem == Member::_load) {
      Data base = op_stk.Pop();
      int32_t addr = base.get<int32_t>();
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
      data.set(static_cast<T1>(val));
      op_stk.Push(data);
    } else if (instr.m_mem == Member::_store) {
      Data val = op_stk.Pop();
      Data base = op_stk.Pop();
      int32_t addr = base.get<int32_t>();
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
      T1 f = val.get<T1>();
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
