#pragma once
#include "types.hpp"
#include <array>
#include <string_view>

struct L1Cache {};

struct L2Cache {};

struct L3Cache {};

struct Function {
  std::string_view m_name;
  uint32_t m_typeIndex = 0;

  std::array<ParamType, 16> m_params{};
  uint32_t m_paramCount = 0;

  std::array<ParamType, 32> m_locals{};
  uint32_t m_localCount = 0;

  std::array<Instr, 256> m_body{};
  uint32_t m_bodyCount = 0;
};

struct FunctionTable {
  std::array<Function, MAXFUNCTIONS> m_data{};
};

struct Type {};

struct Table {};

struct Memory {};

struct Global {
  std::array<Instr, MAXGLOBALS> m_data{};
};

struct Export {};

struct Stack {

  constexpr void Push(const Instr &instr) {
    if (m_stackPointer == 0)
      throw "Stack overflow";
    m_data[--m_stackPointer] = instr;
  }

  constexpr Instr Pop() {
    if (m_stackPointer == STACKSIZE)
      throw "Stack underflow";
    return m_data[m_stackPointer++];
  }

  uint64_t m_basePointer = 0;
  uint64_t m_stackPointer = 0;
  uint64_t m_framePointer = 0;
  std::array<Instr, STACKSIZE> m_data{};
};

struct Heap {};

struct Module {};

struct State {
  Stack m_stack{};
  Global m_global{};
  uint64_t m_instrPointer = 0;
  FunctionTable m_functionTable{};
  Function *m_activeFunction = nullptr;
};

inline constexpr std::string_view program = R"(
(module
  (type (;0;) (func (param i32 i32) (result i32)))
  (type (;1;) (func (result i32)))
  (func $add_int__int_ (type 0) (param i32 i32) (result i32)
    (local i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 2
    i32.const 16
    local.set 3
    local.get 2
    local.get 3
    i32.sub
    local.set 4
    local.get 4
    local.get 0
    i32.store offset=12
    local.get 4
    local.get 1
    i32.store offset=8
    local.get 4
    i32.load offset=12
    local.set 5
    local.get 4
    i32.load offset=8
    local.set 6
    local.get 5
    local.get 6
    i32.add
    local.set 7
    local.get 7
    return)
  (func $sub_int__int_ (type 0) (param i32 i32) (result i32)
    (local i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 2
    i32.const 16
    local.set 3
    local.get 2
    local.get 3
    i32.sub
    local.set 4
    local.get 4
    local.get 0
    i32.store offset=12
    local.get 4
    local.get 1
    i32.store offset=8
    local.get 4
    i32.load offset=12
    local.set 5
    local.get 4
    i32.load offset=8
    local.set 6
    local.get 5
    local.get 6
    i32.sub
    local.set 7
    local.get 7
    return)
  (func $__original_main (type 1) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 0
    i32.const 16
    local.set 1
    local.get 0
    local.get 1
    i32.sub
    local.set 2
    local.get 2
    global.set $__stack_pointer
    i32.const 0
    local.set 3
    local.get 2
    local.get 3
    i32.store offset=12
    i32.const 10
    local.set 4
    local.get 2
    local.get 4
    i32.store offset=8
    i32.const 4
    local.set 5
    local.get 2
    local.get 5
    i32.store offset=4
    local.get 2
    i32.load offset=8
    local.set 6
    local.get 2
    i32.load offset=4
    local.set 7
    local.get 6
    local.get 7
    call $add_int__int_
    local.set 8
    local.get 2
    i32.load offset=8
    local.set 9
    local.get 2
    i32.load offset=4
    local.set 10
    local.get 9
    local.get 10
    call $sub_int__int_
    local.set 11
    local.get 8
    local.get 11
    i32.sub
    local.set 12
    i32.const 16
    local.set 13
    local.get 2
    local.get 13
    i32.add
    local.set 14
    local.get 14
    global.set $__stack_pointer
    local.get 12
    return)
  (func $main (type 0) (param i32 i32) (result i32)
    (local i32)
    call $__original_main
    local.set 2
    local.get 2
    return
  )
  (table (;0;) 1 1 funcref)
  (memory (;0;) 2)
  (global $__stack_pointer (mut i32) (i32.const 66560))
  (export "memory" (memory 0))
  (export "main" (func $main)))
)";