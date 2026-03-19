#pragma once
#include "program.hpp"
#include "types.hpp"
#include <array>
#include <string_view>

struct L1Cache {};

struct L2Cache {};

struct Block {
  uint32_t m_blockStart{};
  uint32_t m_blockEnd{};
  bool m_isLoop{false};
};

struct Function {
  std::string_view m_name;
  uint32_t m_typeIndex = 0;

  std::array<ParamType, MAXNUMPARAMS> m_params{};
  uint32_t m_paramCount = 0;

  std::array<ParamType, 256> m_locals{};
  uint32_t m_localCount = 0;

  std::array<Instr, 256> m_body{};
  uint32_t m_bodyCount = 0;

  std::array<uint32_t, BLOCKSTACKSIZE> m_blockStack{};

  std::array<Block, BLOCKSIZE> m_blockTable{};
  uint32_t m_blockStackPointer = BLOCKSTACKSIZE;
  uint32_t m_blockIdx = 0;

  constexpr Block &getBlock() { return m_blockTable[m_blockIdx++]; }

  constexpr void Push(uint32_t idx) {
    if (m_blockStackPointer == 0)
      throw "Stack overflow";
    m_blockStack[--m_blockStackPointer] = idx;
  }
  constexpr Block &Pop() {
    if (m_blockStackPointer == BLOCKSTACKSIZE)
      throw "Stack underflow";

    uint32_t idx = m_blockStack[m_blockStackPointer++];
    return m_blockTable[idx];
  }
};

struct FunctionTable {
  std::array<Function, MAXFUNCTIONS> m_data{};
};

struct Type {};

struct Table {};

struct Memory {};

struct Global {
  std::array<uint8_t, MAXGLOBALS> m_data{};
};

struct Export {};

struct Stack {

  constexpr void Push(const Data &data) {
    if (m_stackPointer == 0)
      throw "Stack overflow";
    m_data[--m_stackPointer] = data;
  }

  constexpr Data Pop() {
    if (m_stackPointer == 66560)
      throw "Stack underflow";
    return m_data[m_stackPointer++];
  }

  uint64_t m_basePointer = 0;
  uint64_t m_stackPointer = 0;
  uint64_t m_framePointer = 0;
  std::array<Data, STACKSIZE> m_data{};
};

struct Heap {};

struct Module {};

struct State {
  Stack m_stack{};
  Stack m_opStack{};
  Global m_global{};
  uint64_t m_instrPointer = 0;
  FunctionTable m_functionTable{};
  Function *m_activeFunction = nullptr;
};