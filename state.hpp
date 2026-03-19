#pragma once
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

  std::array<ParamType, 32> m_params{};
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

inline constexpr std::string_view program = R"(
(module
  (type (;0;) (func (result i32)))
  (type (;1;) (func (param i32 i32) (result i32)))
  (func $__original_main (type 0) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 0
    i32.const 16
    local.set 1
    local.get 0
    local.get 1
    i32.sub
    local.set 2
    i32.const 0
    local.set 3
    local.get 2
    local.get 3
    i32.store offset=12
    i32.const 6
    local.set 4
    local.get 2
    local.get 4
    i32.store offset=8
    i32.const 0
    local.set 5
    local.get 2
    local.get 5
    i32.store offset=4
    block  ;; label = @1
      loop  ;; label = @2
        local.get 2
        i32.load offset=8
        local.set 6
        i32.const 1
        local.set 7
        local.get 6
        local.set 8
        local.get 7
        local.set 9
        local.get 8
        local.get 9
        i32.ne
        local.set 10
        i32.const 1
        local.set 11
        local.get 10
        local.get 11
        i32.and
        local.set 12
        local.get 12
        i32.eqz
        br_if 1 (;@1;)
        local.get 2
        i32.load offset=8
        local.set 13
        i32.const 2
        local.set 14
        local.get 13
        local.get 14
        i32.rem_s
        local.set 15
        block  ;; label = @3
          block  ;; label = @4
            local.get 15
            br_if 0 (;@4;)
            local.get 2
            i32.load offset=8
            local.set 16
            i32.const 2
            local.set 17
            local.get 16
            local.get 17
            i32.div_s
            local.set 18
            local.get 2
            local.get 18
            i32.store offset=8
            br 1 (;@3;)
          end
          local.get 2
          i32.load offset=8
          local.set 19
          i32.const 3
          local.set 20
          local.get 19
          local.get 20
          i32.mul
          local.set 21
          i32.const 1
          local.set 22
          local.get 21
          local.get 22
          i32.add
          local.set 23
          local.get 2
          local.get 23
          i32.store offset=8
        end
        local.get 2
        i32.load offset=4
        local.set 24
        i32.const 1
        local.set 25
        local.get 24
        local.get 25
        i32.add
        local.set 26
        local.get 2
        local.get 26
        i32.store offset=4
        br 0 (;@2;)
      end
    end
    local.get 2
    i32.load offset=4
    local.set 27
    local.get 27
    return)
  (func $main (type 1) (param i32 i32) (result i32)
    (local i32)
    call $__original_main
    local.set 2
    local.get 2
    return)
  (table (;0;) 1 1 funcref)
  (memory (;0;) 2)
  (global $__stack_pointer (mut i32) (i32.const 66560))
  (export "memory" (memory 0))
  (export "main" (func $main)))
)";