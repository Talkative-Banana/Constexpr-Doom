#pragma once
#include <string_view>

inline constexpr std::string_view program1 = R"(
(module
  (type (;0;) (func (result i32)))
  (type (;1;) (func (param i32 i32) (result i32)))
  (func $__original_main (type 0) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 10
    local.set 4
    local.get 2
    local.get 4
    i32.store offset=8
    i32.const 20
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
    i32.add
    local.set 8
    local.get 8
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

inline constexpr std::string_view program2 = R"(
(module
  (type (;0;) (func (result i32)))
  (type (;1;) (func (param i32 i32) (result i32)))
  (func $__original_main (type 0) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 0
    local.set 4
    local.get 2
    local.get 4
    i32.store offset=8
    i32.const 1
    local.set 5
    local.get 2
    local.get 5
    i32.store offset=4
    block  ;; label = @1
      loop  ;; label = @2
        local.get 2
        i32.load offset=4
        local.set 6
        i32.const 10
        local.set 7
        local.get 6
        local.set 8
        local.get 7
        local.set 9
        local.get 8
        local.get 9
        i32.le_s
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
        i32.load offset=4
        local.set 13
        local.get 2
        i32.load offset=8
        local.set 14
        local.get 14
        local.get 13
        i32.add
        local.set 15
        local.get 2
        local.get 15
        i32.store offset=8
        local.get 2
        i32.load offset=4
        local.set 16
        i32.const 1
        local.set 17
        local.get 16
        local.get 17
        i32.add
        local.set 18
        local.get 2
        local.get 18
        i32.store offset=4
        br 0 (;@2;)
      end
    end
    local.get 2
    i32.load offset=8
    local.set 19
    local.get 19
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

inline constexpr std::string_view program3 = R"(
(module
  (type (;0;) (func (param i32 i32) (result i32)))
  (type (;1;) (func (result i32)))
  (func $multiply_int__int_ (type 0) (param i32 i32) (result i32)
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
    i32.mul
    local.set 7
    local.get 7
    return)
  (func $__original_main (type 1) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 6
    local.set 4
    i32.const 7
    local.set 5
    local.get 4
    local.get 5
    call $multiply_int__int_
    local.set 6
    i32.const 16
    local.set 7
    local.get 2
    local.get 7
    i32.add
    local.set 8
    local.get 8
    global.set $__stack_pointer
    local.get 6
    return)
  (func $main (type 0) (param i32 i32) (result i32)
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

inline constexpr std::string_view program4 = R"(
(module
  (type (;0;) (func (param i32) (result i32)))
  (type (;1;) (func (result i32)))
  (type (;2;) (func (param i32 i32) (result i32)))
  (func $factorial_int_ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 1
    i32.const 16
    local.set 2
    local.get 1
    local.get 2
    i32.sub
    local.set 3
    local.get 3
    global.set $__stack_pointer
    local.get 3
    local.get 0
    i32.store offset=8
    local.get 3
    i32.load offset=8
    local.set 4
    i32.const 1
    local.set 5
    local.get 4
    local.set 6
    local.get 5
    local.set 7
    local.get 6
    local.get 7
    i32.le_s
    local.set 8
    i32.const 1
    local.set 9
    local.get 8
    local.get 9
    i32.and
    local.set 10
    block  ;; label = @1
      block  ;; label = @2
        local.get 10
        i32.eqz
        br_if 0 (;@2;)
        i32.const 1
        local.set 11
        local.get 3
        local.get 11
        i32.store offset=12
        br 1 (;@1;)
      end
      local.get 3
      i32.load offset=8
      local.set 12
      local.get 3
      i32.load offset=8
      local.set 13
      i32.const 1
      local.set 14
      local.get 13
      local.get 14
      i32.sub
      local.set 15
      local.get 15
      call $factorial_int_
      local.set 16
      local.get 12
      local.get 16
      i32.mul
      local.set 17
      local.get 3
      local.get 17
      i32.store offset=12
    end
    local.get 3
    i32.load offset=12
    local.set 18
    i32.const 16
    local.set 19
    local.get 3
    local.get 19
    i32.add
    local.set 20
    local.get 20
    global.set $__stack_pointer
    local.get 18
    return)
  (func $__original_main (type 1) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 8
    local.set 4
    local.get 4
    call $factorial_int_
    local.set 5
    i32.const 16
    local.set 6
    local.get 2
    local.get 6
    i32.add
    local.set 7
    local.get 7
    global.set $__stack_pointer
    local.get 5
    return)
  (func $main (type 2) (param i32 i32) (result i32)
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

inline constexpr std::string_view program5 = R"(
(module
  (type (;0;) (func (param i32) (result i32)))
  (type (;1;) (func (result i32)))
  (type (;2;) (func (param i32 i32) (result i32)))
  (func $fib_int_ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 1
    i32.const 16
    local.set 2
    local.get 1
    local.get 2
    i32.sub
    local.set 3
    local.get 3
    global.set $__stack_pointer
    local.get 3
    local.get 0
    i32.store offset=8
    local.get 3
    i32.load offset=8
    local.set 4
    i32.const 2
    local.set 5
    local.get 4
    local.set 6
    local.get 5
    local.set 7
    local.get 6
    local.get 7
    i32.lt_s
    local.set 8
    i32.const 1
    local.set 9
    local.get 8
    local.get 9
    i32.and
    local.set 10
    block  ;; label = @1
      block  ;; label = @2
        local.get 10
        i32.eqz
        br_if 0 (;@2;)
        local.get 3
        i32.load offset=8
        local.set 11
        local.get 3
        local.get 11
        i32.store offset=12
        br 1 (;@1;)
      end
      local.get 3
      i32.load offset=8
      local.set 12
      i32.const 1
      local.set 13
      local.get 12
      local.get 13
      i32.sub
      local.set 14
      local.get 14
      call $fib_int_
      local.set 15
      local.get 3
      i32.load offset=8
      local.set 16
      i32.const 2
      local.set 17
      local.get 16
      local.get 17
      i32.sub
      local.set 18
      local.get 18
      call $fib_int_
      local.set 19
      local.get 15
      local.get 19
      i32.add
      local.set 20
      local.get 3
      local.get 20
      i32.store offset=12
    end
    local.get 3
    i32.load offset=12
    local.set 21
    i32.const 16
    local.set 22
    local.get 3
    local.get 22
    i32.add
    local.set 23
    local.get 23
    global.set $__stack_pointer
    local.get 21
    return)
  (func $__original_main (type 1) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32)
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
    local.get 4
    call $fib_int_
    local.set 5
    i32.const 16
    local.set 6
    local.get 2
    local.get 6
    i32.add
    local.set 7
    local.get 7
    global.set $__stack_pointer
    local.get 5
    return)
  (func $main (type 2) (param i32 i32) (result i32)
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

// 111
inline constexpr std::string_view program6 = R"(
(module
  (type (;0;) (func (param i32) (result i32)))
  (type (;1;) (func (result i32)))
  (type (;2;) (func (param i32 i32) (result i32)))
  (func $collatz_int_ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 1
    i32.const 16
    local.set 2
    local.get 1
    local.get 2
    i32.sub
    local.set 3
    local.get 3
    local.get 0
    i32.store offset=12
    i32.const 0
    local.set 4
    local.get 3
    local.get 4
    i32.store offset=8
    block  ;; label = @1
      loop  ;; label = @2
        local.get 3
        i32.load offset=12
        local.set 5
        i32.const 1
        local.set 6
        local.get 5
        local.set 7
        local.get 6
        local.set 8
        local.get 7
        local.get 8
        i32.gt_s
        local.set 9
        i32.const 1
        local.set 10
        local.get 9
        local.get 10
        i32.and
        local.set 11
        local.get 11
        i32.eqz
        br_if 1 (;@1;)
        local.get 3
        i32.load offset=12
        local.set 12
        i32.const 2
        local.set 13
        local.get 12
        local.get 13
        i32.rem_s
        local.set 14
        block  ;; label = @3
          block  ;; label = @4
            local.get 14
            br_if 0 (;@4;)
            local.get 3
            i32.load offset=12
            local.set 15
            i32.const 2
            local.set 16
            local.get 15
            local.get 16
            i32.div_s
            local.set 17
            local.get 3
            local.get 17
            i32.store offset=12
            br 1 (;@3;)
          end
          local.get 3
          i32.load offset=12
          local.set 18
          i32.const 3
          local.set 19
          local.get 18
          local.get 19
          i32.mul
          local.set 20
          i32.const 1
          local.set 21
          local.get 20
          local.get 21
          i32.add
          local.set 22
          local.get 3
          local.get 22
          i32.store offset=12
        end
        local.get 3
        i32.load offset=8
        local.set 23
        i32.const 1
        local.set 24
        local.get 23
        local.get 24
        i32.add
        local.set 25
        local.get 3
        local.get 25
        i32.store offset=8
        br 0 (;@2;)
      end
    end
    local.get 3
    i32.load offset=8
    local.set 26
    local.get 26
    return)
  (func $__original_main (type 1) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 27
    local.set 4
    local.get 4
    call $collatz_int_
    local.set 5
    i32.const 16
    local.set 6
    local.get 2
    local.get 6
    i32.add
    local.set 7
    local.get 7
    global.set $__stack_pointer
    local.get 5
    return)
  (func $main (type 2) (param i32 i32) (result i32)
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

// 36
inline constexpr std::string_view program7 = R"(
(module
  (type (;0;) (func (param i32) (result i32)))
  (type (;1;) (func (result i32)))
  (type (;2;) (func (param i32 i32) (result i32)))
  (func $sum_array_int_ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i64 i32 i32 i64 i32 i32 i64 i64 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 1
    i32.const 64
    local.set 2
    local.get 1
    local.get 2
    i32.sub
    local.set 3
    local.get 3
    local.get 0
    i32.store offset=60
    i32.const 16
    local.set 4
    local.get 3
    local.get 4
    i32.add
    local.set 5
    local.get 5
    local.set 6
    i32.const 24
    local.set 7
    local.get 6
    local.get 7
    i32.add
    local.set 8
    i32.const 0
    local.set 9
    local.get 9
    i64.load offset=1048
    local.set 10
    local.get 8
    local.get 10
    i64.store
    i32.const 16
    local.set 11
    local.get 6
    local.get 11
    i32.add
    local.set 12
    local.get 9
    i64.load offset=1040
    local.set 13
    local.get 12
    local.get 13
    i64.store
    i32.const 8
    local.set 14
    local.get 6
    local.get 14
    i32.add
    local.set 15
    local.get 9
    i64.load offset=1032
    local.set 16
    local.get 15
    local.get 16
    i64.store
    local.get 9
    i64.load offset=1024
    local.set 17
    local.get 6
    local.get 17
    i64.store
    i32.const 0
    local.set 18
    local.get 3
    local.get 18
    i32.store offset=12
    i32.const 0
    local.set 19
    local.get 3
    local.get 19
    i32.store offset=8
    block  ;; label = @1
      loop  ;; label = @2
        local.get 3
        i32.load offset=8
        local.set 20
        local.get 3
        i32.load offset=60
        local.set 21
        local.get 20
        local.set 22
        local.get 21
        local.set 23
        local.get 22
        local.get 23
        i32.lt_s
        local.set 24
        i32.const 1
        local.set 25
        local.get 24
        local.get 25
        i32.and
        local.set 26
        local.get 26
        i32.eqz
        br_if 1 (;@1;)
        local.get 3
        i32.load offset=8
        local.set 27
        i32.const 16
        local.set 28
        local.get 3
        local.get 28
        i32.add
        local.set 29
        local.get 29
        local.set 30
        i32.const 2
        local.set 31
        local.get 27
        local.get 31
        i32.shl
        local.set 32
        local.get 30
        local.get 32
        i32.add
        local.set 33
        local.get 33
        i32.load
        local.set 34
        local.get 3
        i32.load offset=12
        local.set 35
        local.get 35
        local.get 34
        i32.add
        local.set 36
        local.get 3
        local.get 36
        i32.store offset=12
        local.get 3
        i32.load offset=8
        local.set 37
        i32.const 1
        local.set 38
        local.get 37
        local.get 38
        i32.add
        local.set 39
        local.get 3
        local.get 39
        i32.store offset=8
        br 0 (;@2;)
      end
    end
    local.get 3
    i32.load offset=12
    local.set 40
    local.get 40
    return)
  (func $__original_main (type 1) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 8
    local.set 4
    local.get 4
    call $sum_array_int_
    local.set 5
    i32.const 16
    local.set 6
    local.get 2
    local.get 6
    i32.add
    local.set 7
    local.get 7
    global.set $__stack_pointer
    local.get 5
    return)
  (func $main (type 2) (param i32 i32) (result i32)
    (local i32)
    call $__original_main
    local.set 2
    local.get 2
    return)
  (table (;0;) 1 1 funcref)
  (memory (;0;) 2)
  (global $__stack_pointer (mut i32) (i32.const 66592))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (data $.rodata (i32.const 1024) "\01\00\00\00\02\00\00\00\03\00\00\00\04\00\00\00\05\00\00\00\06\00\00\00\07\00\00\00\08\00\00\00"))
)";

// 5
inline constexpr std::string_view program8 = R"(
(module
  (type (;0;) (func (param i32) (result i32)))
  (type (;1;) (func (result i32)))
  (type (;2;) (func (param i32 i32) (result i32)))
  (func $isEven_int_ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 1
    i32.const 16
    local.set 2
    local.get 1
    local.get 2
    i32.sub
    local.set 3
    local.get 3
    local.get 0
    i32.store offset=12
    local.get 3
    i32.load offset=12
    local.set 4
    i32.const 2
    local.set 5
    local.get 4
    local.get 5
    i32.rem_s
    local.set 6
    i32.const 0
    local.set 7
    local.get 6
    local.set 8
    local.get 7
    local.set 9
    local.get 8
    local.get 9
    i32.eq
    local.set 10
    i32.const 1
    local.set 11
    local.get 10
    local.get 11
    i32.and
    local.set 12
    local.get 12
    return)
  (func $isOdd_int_ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 1
    i32.const 16
    local.set 2
    local.get 1
    local.get 2
    i32.sub
    local.set 3
    local.get 3
    global.set $__stack_pointer
    local.get 3
    local.get 0
    i32.store offset=12
    local.get 3
    i32.load offset=12
    local.set 4
    local.get 4
    call $isEven_int_
    local.set 5
    i32.const 0
    local.set 6
    local.get 5
    local.set 7
    local.get 6
    local.set 8
    local.get 7
    local.get 8
    i32.ne
    local.set 9
    i32.const -1
    local.set 10
    local.get 9
    local.get 10
    i32.xor
    local.set 11
    i32.const 1
    local.set 12
    local.get 11
    local.get 12
    i32.and
    local.set 13
    i32.const 16
    local.set 14
    local.get 3
    local.get 14
    i32.add
    local.set 15
    local.get 15
    global.set $__stack_pointer
    local.get 13
    return)
  (func $countOdd_int_ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 1
    i32.const 16
    local.set 2
    local.get 1
    local.get 2
    i32.sub
    local.set 3
    local.get 3
    global.set $__stack_pointer
    local.get 3
    local.get 0
    i32.store offset=12
    i32.const 0
    local.set 4
    local.get 3
    local.get 4
    i32.store offset=8
    i32.const 1
    local.set 5
    local.get 3
    local.get 5
    i32.store offset=4
    block  ;; label = @1
      loop  ;; label = @2
        local.get 3
        i32.load offset=4
        local.set 6
        local.get 3
        i32.load offset=12
        local.set 7
        local.get 6
        local.set 8
        local.get 7
        local.set 9
        local.get 8
        local.get 9
        i32.le_s
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
        local.get 3
        i32.load offset=4
        local.set 13
        local.get 13
        call $isOdd_int_
        local.set 14
        block  ;; label = @3
          local.get 14
          i32.eqz
          br_if 0 (;@3;)
          local.get 3
          i32.load offset=8
          local.set 15
          i32.const 1
          local.set 16
          local.get 15
          local.get 16
          i32.add
          local.set 17
          local.get 3
          local.get 17
          i32.store offset=8
        end
        local.get 3
        i32.load offset=4
        local.set 18
        i32.const 1
        local.set 19
        local.get 18
        local.get 19
        i32.add
        local.set 20
        local.get 3
        local.get 20
        i32.store offset=4
        br 0 (;@2;)
      end
    end
    local.get 3
    i32.load offset=8
    local.set 21
    i32.const 16
    local.set 22
    local.get 3
    local.get 22
    i32.add
    local.set 23
    local.get 23
    global.set $__stack_pointer
    local.get 21
    return)
  (func $__original_main (type 1) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 9
    local.set 4
    local.get 4
    call $countOdd_int_
    local.set 5
    i32.const 16
    local.set 6
    local.get 2
    local.get 6
    i32.add
    local.set 7
    local.get 7
    global.set $__stack_pointer
    local.get 5
    return)
  (func $main (type 2) (param i32 i32) (result i32)
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

// 34
inline constexpr std::string_view program9 = R"(
(module
  (type (;0;) (func (result i32)))
  (type (;1;) (func (param i32 i32) (result i32)))
  (func $__original_main (type 0) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 0
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
        i32.load offset=4
        local.set 6
        i32.const 10
        local.set 7
        local.get 6
        local.set 8
        local.get 7
        local.set 9
        local.get 8
        local.get 9
        i32.lt_s
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
        i32.const 0
        local.set 13
        local.get 2
        local.get 13
        i32.store
        block  ;; label = @3
          loop  ;; label = @4
            local.get 2
            i32.load
            local.set 14
            i32.const 10
            local.set 15
            local.get 14
            local.set 16
            local.get 15
            local.set 17
            local.get 16
            local.get 17
            i32.lt_s
            local.set 18
            i32.const 1
            local.set 19
            local.get 18
            local.get 19
            i32.and
            local.set 20
            local.get 20
            i32.eqz
            br_if 1 (;@3;)
            local.get 2
            i32.load offset=4
            local.set 21
            local.get 2
            i32.load
            local.set 22
            local.get 21
            local.get 22
            i32.add
            local.set 23
            i32.const 3
            local.set 24
            local.get 23
            local.get 24
            i32.rem_s
            local.set 25
            block  ;; label = @5
              local.get 25
              br_if 0 (;@5;)
              local.get 2
              i32.load offset=8
              local.set 26
              i32.const 1
              local.set 27
              local.get 26
              local.get 27
              i32.add
              local.set 28
              local.get 2
              local.get 28
              i32.store offset=8
            end
            local.get 2
            i32.load
            local.set 29
            i32.const 1
            local.set 30
            local.get 29
            local.get 30
            i32.add
            local.set 31
            local.get 2
            local.get 31
            i32.store
            br 0 (;@4;)
          end
        end
        local.get 2
        i32.load offset=4
        local.set 32
        i32.const 1
        local.set 33
        local.get 32
        local.get 33
        i32.add
        local.set 34
        local.get 2
        local.get 34
        i32.store offset=4
        br 0 (;@2;)
      end
    end
    local.get 2
    i32.load offset=8
    local.set 35
    local.get 35
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

// 125 
inline constexpr std::string_view program10 = R"(
(module
  (type (;0;) (func (param i32 i32) (result i32)))
  (type (;1;) (func (result i32)))
  (func $ackermann_int__int_ (type 0) (param i32 i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 2
    i32.const 16
    local.set 3
    local.get 2
    local.get 3
    i32.sub
    local.set 4
    local.get 4
    global.set $__stack_pointer
    local.get 4
    local.get 0
    i32.store offset=8
    local.get 4
    local.get 1
    i32.store offset=4
    local.get 4
    i32.load offset=8
    local.set 5
    block  ;; label = @1
      block  ;; label = @2
        local.get 5
        br_if 0 (;@2;)
        local.get 4
        i32.load offset=4
        local.set 6
        i32.const 1
        local.set 7
        local.get 6
        local.get 7
        i32.add
        local.set 8
        local.get 4
        local.get 8
        i32.store offset=12
        br 1 (;@1;)
      end
      local.get 4
      i32.load offset=4
      local.set 9
      block  ;; label = @2
        local.get 9
        br_if 0 (;@2;)
        local.get 4
        i32.load offset=8
        local.set 10
        i32.const 1
        local.set 11
        local.get 10
        local.get 11
        i32.sub
        local.set 12
        i32.const 1
        local.set 13
        local.get 12
        local.get 13
        call $ackermann_int__int_
        local.set 14
        local.get 4
        local.get 14
        i32.store offset=12
        br 1 (;@1;)
      end
      local.get 4
      i32.load offset=8
      local.set 15
      i32.const 1
      local.set 16
      local.get 15
      local.get 16
      i32.sub
      local.set 17
      local.get 4
      i32.load offset=8
      local.set 18
      local.get 4
      i32.load offset=4
      local.set 19
      i32.const 1
      local.set 20
      local.get 19
      local.get 20
      i32.sub
      local.set 21
      local.get 18
      local.get 21
      call $ackermann_int__int_
      local.set 22
      local.get 17
      local.get 22
      call $ackermann_int__int_
      local.set 23
      local.get 4
      local.get 23
      i32.store offset=12
    end
    local.get 4
    i32.load offset=12
    local.set 24
    i32.const 16
    local.set 25
    local.get 4
    local.get 25
    i32.add
    local.set 26
    local.get 26
    global.set $__stack_pointer
    local.get 24
    return)
  (func $__original_main (type 1) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 3
    local.set 4
    i32.const 4
    local.set 5
    local.get 4
    local.get 5
    call $ackermann_int__int_
    local.set 6
    i32.const 16
    local.set 7
    local.get 2
    local.get 7
    i32.add
    local.set 8
    local.get 8
    global.set $__stack_pointer
    local.get 6
    return)
  (func $main (type 0) (param i32 i32) (result i32)
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

inline constexpr std::string_view program = R"(
(module
  (type (;0;) (func (result i32)))
  (type (;1;) (func (param i32 i32) (result i32)))
  (func $__original_main (type 0) (result i32)
    (local i32 i32 i32 i32 f32 f32 f32 f32 f32 i32)
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
    f32.const 0x1.866666p+3 (;=12.2;)
    local.set 4
    local.get 2
    local.get 4
    f32.store offset=8
    f32.const 0x1.bp+3 (;=13.5;)
    local.set 5
    local.get 2
    local.get 5
    f32.store offset=4
    local.get 2
    f32.load offset=8
    local.set 6
    local.get 2
    f32.load offset=4
    local.set 7
    local.get 6
    local.get 7
    f32.add
    local.set 8
    local.get 2
    local.get 8
    f32.store
    i32.const 0
    local.set 9
    local.get 9
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