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
  int m_count{};
  std::array<Function, MAXFUNCTIONS> m_data{};
};

struct Type {};

struct VirtualTable {
  int m_count{};
  std::array<Function *, MAXVIRTUALTABLESIZE> m_data{};
};

struct Memory {
  std::array<uint8_t, MEMORYSIZE> m_data{};
};

struct Global {
  int m_count{};
  std::array<Data, GLOBALSIZE> m_data{};
};

struct Export {};

struct Stack {

  // Grow the stack downwards
  constexpr void Push(const Data &data) {
    if (m_stackPointer == STACKSIZE)
      throw "Stack overflow";
    m_data[++m_stackPointer] = data;
  }

  constexpr Data Pop() {
    if (m_stackPointer == m_floorPointer)
      throw "Stack underflow";
    return m_data[m_stackPointer--];
  }

  uint64_t m_basePointer = 0;
  uint64_t m_stackPointer = 0;
  uint64_t m_framePointer = 0;
  uint64_t m_floorPointer = 0;
  std::array<Data, STACKSIZE> m_data{};
};

struct Heap {};

struct Module {};

struct State {
  Stack m_stack{};
  Stack m_opStack{};
  Global m_global{};
  Memory m_memory{};
  uint64_t m_instrPointer = 0;
  FunctionTable m_functionTable{};
  VirtualTable m_virtualTable{};
  Function *m_activeFunction = nullptr;
};