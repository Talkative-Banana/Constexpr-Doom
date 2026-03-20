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

// 18
inline constexpr std::string_view program11 = R"(
(module
  (type (;0;) (func (result i32)))
  (type (;1;) (func (param i32 i32) (result i32)))
  (func $__original_main (type 0) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 13
    local.set 4
    local.get 2
    local.get 4
    i32.store offset=8
    i32.const 0
    local.set 5
    local.get 5
    i32.load offset=1024
    local.set 6
    local.get 2
    i32.load offset=8
    local.set 7
    local.get 6
    local.get 7
    i32.add
    local.set 8
    local.get 2
    local.get 8
    i32.store offset=4
    local.get 2
    i32.load offset=4
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
  (global $__stack_pointer (mut i32) (i32.const 66576))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (data $.data (i32.const 1024) "\05\00\00\00"))
)";

// 20
inline constexpr std::string_view program12 = R"(
(module
  (type (;0;) (func (param i32) (result i32)))
  (type (;1;) (func))
  (type (;2;) (func (result i32)))
  (type (;3;) (func (param i32 i32) (result i32)))
  (func $__cxa_pure_virtual (type 1)
    loop  ;; label = @1
      br 0 (;@1;)
    end)
  (func $__original_main (type 2) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    local.get 2
    local.get 4
    i32.add
    local.set 5
    local.get 5
    local.set 6
    local.get 6
    call $B::B__
    drop
    local.get 2
    local.set 7
    local.get 7
    call $C::C__
    drop
    i32.const 8
    local.set 8
    local.get 2
    local.get 8
    i32.add
    local.set 9
    local.get 9
    local.set 10
    local.get 10
    call $A::doubled__
    local.set 11
    local.get 2
    local.set 12
    local.get 12
    call $A::doubled__
    local.set 13
    local.get 11
    local.get 13
    i32.add
    local.set 14
    i32.const 16
    local.set 15
    local.get 2
    local.get 15
    i32.add
    local.set 16
    local.get 16
    global.set $__stack_pointer
    local.get 14
    return)
  (func $B::B__ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    call $A::A__
    drop
    i32.const 1024
    local.set 5
    i32.const 8
    local.set 6
    local.get 5
    local.get 6
    i32.add
    local.set 7
    local.get 7
    local.set 8
    local.get 4
    local.get 8
    i32.store
    i32.const 16
    local.set 9
    local.get 3
    local.get 9
    i32.add
    local.set 10
    local.get 10
    global.set $__stack_pointer
    local.get 4
    return)
  (func $C::C__ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    call $A::A__
    drop
    i32.const 1048
    local.set 5
    i32.const 8
    local.set 6
    local.get 5
    local.get 6
    i32.add
    local.set 7
    local.get 7
    local.set 8
    local.get 4
    local.get 8
    i32.store
    i32.const 16
    local.set 9
    local.get 3
    local.get 9
    i32.add
    local.set 10
    local.get 10
    global.set $__stack_pointer
    local.get 4
    return)
  (func $A::doubled__ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.load
    local.set 5
    local.get 5
    i32.load
    local.set 6
    local.get 4
    local.get 6
    call_indirect (type 0)
    local.set 7
    i32.const 1
    local.set 8
    local.get 7
    local.get 8
    i32.shl
    local.set 9
    i32.const 16
    local.set 10
    local.get 3
    local.get 10
    i32.add
    local.set 11
    local.get 11
    global.set $__stack_pointer
    local.get 9
    return)
  (func $A::A__ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 1036
    local.set 5
    i32.const 8
    local.set 6
    local.get 5
    local.get 6
    i32.add
    local.set 7
    local.get 7
    local.set 8
    local.get 4
    local.get 8
    i32.store
    local.get 4
    return)
  (func $B::value__ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32)
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
    i32.const 3
    local.set 4
    local.get 4
    return)
  (func $C::value__ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32)
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
    i32.const 7
    local.set 4
    local.get 4
    return)
  (func $main (type 3) (param i32 i32) (result i32)
    (local i32)
    call $__original_main
    local.set 2
    local.get 2
    return)
  (table (;0;) 4 4 funcref)
  (memory (;0;) 2)
  (global $__stack_pointer (mut i32) (i32.const 66608))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (elem (;0;) (i32.const 1) func $B::value__ $__cxa_pure_virtual $C::value__)
  (data $.rodata (i32.const 1024) "\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\02\00\00\00\00\00\00\00\00\00\00\00\03\00\00\00"))
)";

// 25
inline constexpr std::string_view program13 = R"(
(module
  (type (;0;) (func (param i32) (result i32)))
  (type (;1;) (func (result i32)))
  (type (;2;) (func (param i32 i32) (result i32)))
  (type (;3;) (func (param i32 i32 i32) (result i32)))
  (func $__original_main (type 1) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 0
    i32.const 32
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
    i32.store offset=28
    i32.const 16
    local.set 4
    local.get 2
    local.get 4
    i32.add
    local.set 5
    local.get 5
    local.set 6
    i32.const 5
    local.set 7
    local.get 6
    local.get 7
    call $Square::Square_int_
    drop
    i32.const 16
    local.set 8
    local.get 2
    local.get 8
    i32.add
    local.set 9
    local.get 9
    local.set 10
    local.get 2
    local.get 10
    i32.store offset=12
    local.get 2
    i32.load offset=12
    local.set 11
    local.get 11
    i32.load
    local.set 12
    local.get 12
    i32.load
    local.set 13
    local.get 11
    local.get 13
    call_indirect (type 0)
    local.set 14
    i32.const 32
    local.set 15
    local.get 2
    local.get 15
    i32.add
    local.set 16
    local.get 16
    global.set $__stack_pointer
    local.get 14
    return)
  (func $Square::Square_int_ (type 2) (param i32 i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    local.get 4
    i32.load offset=8
    local.set 7
    local.get 5
    local.get 6
    local.get 7
    call $Rectangle::Rectangle_int__int_
    drop
    i32.const 1024
    local.set 8
    i32.const 8
    local.set 9
    local.get 8
    local.get 9
    i32.add
    local.set 10
    local.get 10
    local.set 11
    local.get 5
    local.get 11
    i32.store
    i32.const 16
    local.set 12
    local.get 4
    local.get 12
    i32.add
    local.set 13
    local.get 13
    global.set $__stack_pointer
    local.get 5
    return)
  (func $Rectangle::Rectangle_int__int_ (type 3) (param i32 i32 i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 3
    i32.const 16
    local.set 4
    local.get 3
    local.get 4
    i32.sub
    local.set 5
    local.get 5
    global.set $__stack_pointer
    local.get 5
    local.get 0
    i32.store offset=12
    local.get 5
    local.get 1
    i32.store offset=8
    local.get 5
    local.get 2
    i32.store offset=4
    local.get 5
    i32.load offset=12
    local.set 6
    local.get 6
    call $Shape::Shape__
    drop
    i32.const 1036
    local.set 7
    i32.const 8
    local.set 8
    local.get 7
    local.get 8
    i32.add
    local.set 9
    local.get 9
    local.set 10
    local.get 6
    local.get 10
    i32.store
    local.get 5
    i32.load offset=8
    local.set 11
    local.get 6
    local.get 11
    i32.store offset=4
    local.get 5
    i32.load offset=4
    local.set 12
    local.get 6
    local.get 12
    i32.store offset=8
    i32.const 16
    local.set 13
    local.get 5
    local.get 13
    i32.add
    local.set 14
    local.get 14
    global.set $__stack_pointer
    local.get 6
    return)
  (func $Shape::Shape__ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 1048
    local.set 5
    i32.const 8
    local.set 6
    local.get 5
    local.get 6
    i32.add
    local.set 7
    local.get 7
    local.set 8
    local.get 4
    local.get 8
    i32.store
    local.get 4
    return)
  (func $Rectangle::area__ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32)
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
    local.get 4
    i32.load offset=4
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
  (func $Shape::area__ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32)
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
    local.get 4
    return)
  (func $main (type 2) (param i32 i32) (result i32)
    (local i32)
    call $__original_main
    local.set 2
    local.get 2
    return)
  (table (;0;) 3 3 funcref)
  (memory (;0;) 2)
  (global $__stack_pointer (mut i32) (i32.const 66608))
  (export "memory" (memory 0))
  (export "main" (func $main))
  (elem (;0;) (i32.const 1) func $Rectangle::area__ $Shape::area__)
  (data $.rodata (i32.const 1024) "\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\02\00\00\00"))
)";

// 3
inline constexpr std::string_view program14 = R"(
(module
  (type (;0;) (func (result i32)))
  (type (;1;) (func (param i32) (result i32)))
  (type (;2;) (func (param i32)))
  (type (;3;) (func (param i32 i32) (result i32)))
  (func $__original_main (type 0) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    local.get 2
    local.get 4
    i32.add
    local.set 5
    local.get 5
    local.set 6
    local.get 6
    call $Counter::Counter__
    drop
    i32.const 8
    local.set 7
    local.get 2
    local.get 7
    i32.add
    local.set 8
    local.get 8
    local.set 9
    local.get 9
    call $Counter::increment__
    i32.const 8
    local.set 10
    local.get 2
    local.get 10
    i32.add
    local.set 11
    local.get 11
    local.set 12
    local.get 12
    call $Counter::increment__
    i32.const 8
    local.set 13
    local.get 2
    local.get 13
    i32.add
    local.set 14
    local.get 14
    local.set 15
    local.get 15
    call $Counter::increment__
    i32.const 8
    local.set 16
    local.get 2
    local.get 16
    i32.add
    local.set 17
    local.get 17
    local.set 18
    local.get 18
    call $Counter::get__
    local.set 19
    i32.const 16
    local.set 20
    local.get 2
    local.get 20
    i32.add
    local.set 21
    local.get 21
    global.set $__stack_pointer
    local.get 19
    return)
  (func $Counter::Counter__ (type 1) (param i32) (result i32)
    (local i32 i32 i32 i32 i32)
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
    i32.const 0
    local.set 5
    local.get 4
    local.get 5
    i32.store
    local.get 4
    return)
  (func $Counter::increment__ (type 2) (param i32)
    (local i32 i32 i32 i32 i32 i32 i32)
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
    local.get 4
    i32.load
    local.set 5
    i32.const 1
    local.set 6
    local.get 5
    local.get 6
    i32.add
    local.set 7
    local.get 4
    local.get 7
    i32.store
    return)
  (func $Counter::get__ (type 1) (param i32) (result i32)
    (local i32 i32 i32 i32 i32)
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
    local.get 4
    i32.load
    local.set 5
    local.get 5
    return)
  (func $main (type 3) (param i32 i32) (result i32)
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

// 17
inline constexpr std::string_view program15 = R"(
(module
  (type (;0;) (func (result i32)))
  (type (;1;) (func (param i32 i32) (result i32)))
  (func $__original_main (type 0) (result i32)
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
    i32.const 3
    local.set 4
    i32.const 7
    local.set 5
    local.get 4
    local.get 5
    call $int_max<int>_int__int_
    local.set 6
    local.get 2
    local.get 6
    i32.store offset=8
    i32.const 10
    local.set 7
    i32.const 4
    local.set 8
    local.get 7
    local.get 8
    call $int_max<int>_int__int_
    local.set 9
    local.get 2
    local.get 9
    i32.store offset=4
    local.get 2
    i32.load offset=8
    local.set 10
    local.get 2
    i32.load offset=4
    local.set 11
    local.get 10
    local.get 11
    i32.add
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
  (func $int_max<int>_int__int_ (type 1) (param i32 i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    block  ;; label = @1
      block  ;; label = @2
        local.get 11
        i32.eqz
        br_if 0 (;@2;)
        local.get 4
        i32.load offset=12
        local.set 12
        local.get 12
        local.set 13
        br 1 (;@1;)
      end
      local.get 4
      i32.load offset=8
      local.set 14
      local.get 14
      local.set 13
    end
    local.get 13
    local.set 15
    local.get 15
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

// 6
inline constexpr std::string_view program16 = R"(
(module
  (type (;0;) (func (result i32)))
  (type (;1;) (func (param i32 i32) (result i32)))
  (func $__original_main (type 0) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 0
    i32.const 48
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
    i32.store offset=44
    i32.const 32
    local.set 4
    local.get 2
    local.get 4
    i32.add
    local.set 5
    local.get 5
    local.set 6
    i32.const 1
    local.set 7
    local.get 6
    local.get 7
    call $Node::Node_int_
    drop
    i32.const 24
    local.set 8
    local.get 2
    local.get 8
    i32.add
    local.set 9
    local.get 9
    local.set 10
    i32.const 2
    local.set 11
    local.get 10
    local.get 11
    call $Node::Node_int_
    drop
    i32.const 16
    local.set 12
    local.get 2
    local.get 12
    i32.add
    local.set 13
    local.get 13
    local.set 14
    i32.const 3
    local.set 15
    local.get 14
    local.get 15
    call $Node::Node_int_
    drop
    i32.const 24
    local.set 16
    local.get 2
    local.get 16
    i32.add
    local.set 17
    local.get 17
    local.set 18
    local.get 2
    local.get 18
    i32.store offset=36
    i32.const 16
    local.set 19
    local.get 2
    local.get 19
    i32.add
    local.set 20
    local.get 20
    local.set 21
    local.get 2
    local.get 21
    i32.store offset=28
    i32.const 0
    local.set 22
    local.get 2
    local.get 22
    i32.store offset=12
    i32.const 32
    local.set 23
    local.get 2
    local.get 23
    i32.add
    local.set 24
    local.get 24
    local.set 25
    local.get 2
    local.get 25
    i32.store offset=8
    block  ;; label = @1
      loop  ;; label = @2
        local.get 2
        i32.load offset=8
        local.set 26
        i32.const 0
        local.set 27
        local.get 26
        local.set 28
        local.get 27
        local.set 29
        local.get 28
        local.get 29
        i32.ne
        local.set 30
        i32.const 1
        local.set 31
        local.get 30
        local.get 31
        i32.and
        local.set 32
        local.get 32
        i32.eqz
        br_if 1 (;@1;)
        local.get 2
        i32.load offset=8
        local.set 33
        local.get 33
        i32.load
        local.set 34
        local.get 2
        i32.load offset=12
        local.set 35
        local.get 35
        local.get 34
        i32.add
        local.set 36
        local.get 2
        local.get 36
        i32.store offset=12
        local.get 2
        i32.load offset=8
        local.set 37
        local.get 37
        i32.load offset=4
        local.set 38
        local.get 2
        local.get 38
        i32.store offset=8
        br 0 (;@2;)
      end
    end
    local.get 2
    i32.load offset=12
    local.set 39
    i32.const 48
    local.set 40
    local.get 2
    local.get 40
    i32.add
    local.set 41
    local.get 41
    global.set $__stack_pointer
    local.get 39
    return)
  (func $Node::Node_int_ (type 1) (param i32 i32) (result i32)
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
    i32.store
    i32.const 0
    local.set 7
    local.get 5
    local.get 7
    i32.store offset=4
    local.get 5
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

// 52
inline constexpr std::string_view program17 = R"(
(module
  (type (;0;) (func (result i32)))
  (type (;1;) (func (param i32 i32 i32) (result i32)))
  (type (;2;) (func (param i32 i32 i32)))
  (type (;3;) (func (param i32) (result i32)))
  (type (;4;) (func (param i32 i32) (result i32)))
  (func $__original_main (type 0) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 0
    i32.const 32
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
    i32.store offset=28
    i32.const 16
    local.set 4
    local.get 2
    local.get 4
    i32.add
    local.set 5
    local.get 5
    local.set 6
    i32.const 3
    local.set 7
    i32.const 4
    local.set 8
    local.get 6
    local.get 7
    local.get 8
    call $Vec2::Vec2_int__int_
    drop
    i32.const 8
    local.set 9
    local.get 2
    local.get 9
    i32.add
    local.set 10
    local.get 10
    local.set 11
    i32.const 1
    local.set 12
    i32.const 2
    local.set 13
    local.get 11
    local.get 12
    local.get 13
    call $Vec2::Vec2_int__int_
    drop
    local.get 2
    local.set 14
    i32.const 16
    local.set 15
    local.get 2
    local.get 15
    i32.add
    local.set 16
    local.get 16
    local.set 17
    i32.const 8
    local.set 18
    local.get 2
    local.get 18
    i32.add
    local.set 19
    local.get 19
    local.set 20
    local.get 14
    local.get 17
    local.get 20
    call $Vec2::operator+_Vec2_const&__const
    local.get 2
    local.set 21
    local.get 21
    call $Vec2::length_sq__
    local.set 22
    i32.const 32
    local.set 23
    local.get 2
    local.get 23
    i32.add
    local.set 24
    local.get 24
    global.set $__stack_pointer
    local.get 22
    return)
  (func $Vec2::Vec2_int__int_ (type 1) (param i32 i32 i32) (result i32)
    (local i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 3
    i32.const 16
    local.set 4
    local.get 3
    local.get 4
    i32.sub
    local.set 5
    local.get 5
    local.get 0
    i32.store offset=12
    local.get 5
    local.get 1
    i32.store offset=8
    local.get 5
    local.get 2
    i32.store offset=4
    local.get 5
    i32.load offset=12
    local.set 6
    local.get 5
    i32.load offset=8
    local.set 7
    local.get 6
    local.get 7
    i32.store
    local.get 5
    i32.load offset=4
    local.set 8
    local.get 6
    local.get 8
    i32.store offset=4
    local.get 6
    return)
  (func $Vec2::operator+_Vec2_const&__const (type 2) (param i32 i32 i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 3
    i32.const 16
    local.set 4
    local.get 3
    local.get 4
    i32.sub
    local.set 5
    local.get 5
    global.set $__stack_pointer
    local.get 5
    local.get 1
    i32.store offset=12
    local.get 5
    local.get 2
    i32.store offset=8
    local.get 5
    i32.load offset=12
    local.set 6
    local.get 6
    i32.load
    local.set 7
    local.get 5
    i32.load offset=8
    local.set 8
    local.get 8
    i32.load
    local.set 9
    local.get 7
    local.get 9
    i32.add
    local.set 10
    local.get 6
    i32.load offset=4
    local.set 11
    local.get 5
    i32.load offset=8
    local.set 12
    local.get 12
    i32.load offset=4
    local.set 13
    local.get 11
    local.get 13
    i32.add
    local.set 14
    local.get 0
    local.get 10
    local.get 14
    call $Vec2::Vec2_int__int_
    drop
    i32.const 16
    local.set 15
    local.get 5
    local.get 15
    i32.add
    local.set 16
    local.get 16
    global.set $__stack_pointer
    return)
  (func $Vec2::length_sq__ (type 3) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    local.get 4
    i32.load
    local.set 5
    local.get 4
    i32.load
    local.set 6
    local.get 5
    local.get 6
    i32.mul
    local.set 7
    local.get 4
    i32.load offset=4
    local.set 8
    local.get 4
    i32.load offset=4
    local.set 9
    local.get 8
    local.get 9
    i32.mul
    local.set 10
    local.get 7
    local.get 10
    i32.add
    local.set 11
    local.get 11
    return)
  (func $main (type 4) (param i32 i32) (result i32)
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

// 1
inline constexpr std::string_view program18 = R"(
(module
  (type (;0;) (func (result i32)))
  (type (;1;) (func (param i32) (result i32)))
  (type (;2;) (func (param i32 i32)))
  (type (;3;) (func (param i32 i32) (result i32)))
  (func $__original_main (type 0) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 0
    i32.const 32
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
    i32.store offset=28
    i32.const 8
    local.set 4
    local.get 2
    local.get 4
    i32.add
    local.set 5
    local.get 5
    local.set 6
    local.get 6
    call $Monster::Monster__
    drop
    i32.const 0
    local.set 7
    local.get 2
    local.get 7
    i32.store offset=4
    block  ;; label = @1
      loop  ;; label = @2
        local.get 2
        i32.load offset=4
        local.set 8
        i32.const 20
        local.set 9
        local.get 8
        local.set 10
        local.get 9
        local.set 11
        local.get 10
        local.get 11
        i32.lt_s
        local.set 12
        i32.const 1
        local.set 13
        local.get 12
        local.get 13
        i32.and
        local.set 14
        local.get 14
        i32.eqz
        br_if 1 (;@1;)
        i32.const 8
        local.set 15
        local.get 2
        local.get 15
        i32.add
        local.set 16
        local.get 16
        local.set 17
        i32.const 5
        local.set 18
        local.get 17
        local.get 18
        call $Monster::update_int_
        local.get 2
        i32.load offset=4
        local.set 19
        i32.const 1
        local.set 20
        local.get 19
        local.get 20
        i32.add
        local.set 21
        local.get 2
        local.get 21
        i32.store offset=4
        br 0 (;@2;)
      end
    end
    local.get 2
    i32.load offset=8
    local.set 22
    i32.const 3
    local.set 23
    local.get 22
    local.set 24
    local.get 23
    local.set 25
    local.get 24
    local.get 25
    i32.eq
    local.set 26
    i32.const 1
    local.set 27
    i32.const 0
    local.set 28
    i32.const 1
    local.set 29
    local.get 26
    local.get 29
    i32.and
    local.set 30
    local.get 27
    local.get 28
    local.get 30
    select
    local.set 31
    i32.const 32
    local.set 32
    local.get 2
    local.get 32
    i32.add
    local.set 33
    local.get 33
    global.set $__stack_pointer
    local.get 31
    return)
  (func $Monster::Monster__ (type 1) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 0
    local.set 5
    local.get 4
    local.get 5
    i32.store
    i32.const 100
    local.set 6
    local.get 4
    local.get 6
    i32.store offset=4
    i32.const 0
    local.set 7
    local.get 4
    local.get 7
    i32.store offset=8
    i32.const 0
    local.set 8
    local.get 4
    local.get 8
    i32.store offset=12
    local.get 4
    return)
  (func $Monster::update_int_ (type 2) (param i32 i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    local.get 5
    i32.load
    local.set 6
    i32.const 3
    local.set 7
    local.get 6
    local.get 7
    i32.gt_u
    drop
    block  ;; label = @1
      block  ;; label = @2
        block  ;; label = @3
          block  ;; label = @4
            block  ;; label = @5
              local.get 6
              br_table 0 (;@5;) 1 (;@4;) 2 (;@3;) 3 (;@2;) 4 (;@1;)
            end
            local.get 4
            i32.load offset=8
            local.set 8
            i32.const 10
            local.set 9
            local.get 8
            local.set 10
            local.get 9
            local.set 11
            local.get 10
            local.get 11
            i32.lt_s
            local.set 12
            i32.const 1
            local.set 13
            local.get 12
            local.get 13
            i32.and
            local.set 14
            block  ;; label = @5
              local.get 14
              i32.eqz
              br_if 0 (;@5;)
              i32.const 1
              local.set 15
              local.get 5
              local.get 15
              i32.store
            end
            br 3 (;@1;)
          end
          local.get 5
          i32.load offset=8
          local.set 16
          i32.const 1
          local.set 17
          local.get 16
          local.get 17
          i32.add
          local.set 18
          local.get 5
          local.get 18
          i32.store offset=8
          local.get 5
          i32.load offset=8
          local.set 19
          local.get 4
          i32.load offset=8
          local.set 20
          local.get 19
          local.set 21
          local.get 20
          local.set 22
          local.get 21
          local.get 22
          i32.ge_s
          local.set 23
          i32.const 1
          local.set 24
          local.get 23
          local.get 24
          i32.and
          local.set 25
          block  ;; label = @4
            local.get 25
            i32.eqz
            br_if 0 (;@4;)
            i32.const 2
            local.set 26
            local.get 5
            local.get 26
            i32.store
          end
          br 2 (;@1;)
        end
        local.get 5
        i32.load offset=4
        local.set 27
        i32.const 10
        local.set 28
        local.get 27
        local.get 28
        i32.sub
        local.set 29
        local.get 5
        local.get 29
        i32.store offset=4
        local.get 5
        i32.load offset=4
        local.set 30
        i32.const 0
        local.set 31
        local.get 30
        local.set 32
        local.get 31
        local.set 33
        local.get 32
        local.get 33
        i32.le_s
        local.set 34
        i32.const 1
        local.set 35
        local.get 34
        local.get 35
        i32.and
        local.set 36
        block  ;; label = @3
          local.get 36
          i32.eqz
          br_if 0 (;@3;)
          i32.const 3
          local.set 37
          local.get 5
          local.get 37
          i32.store
        end
        br 1 (;@1;)
      end
    end
    return)
  (func $main (type 3) (param i32 i32) (result i32)
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

// 6
inline constexpr std::string_view program19 = R"(
(module
  (type (;0;) (func (param i32 i32) (result i32)))
  (type (;1;) (func (result i32)))
  (func $FixedMul_int__int_ (type 0) (param i32 i32) (result i32)
    (local i32 i32 i32 i32 i32 i64 i32 i32 i64 i64 i64 i64 i32)
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
    local.get 5
    local.set 6
    local.get 6
    i64.extend_i32_s
    local.set 7
    local.get 4
    i32.load offset=8
    local.set 8
    local.get 8
    local.set 9
    local.get 9
    i64.extend_i32_s
    local.set 10
    local.get 7
    local.get 10
    i64.mul
    local.set 11
    i64.const 16
    local.set 12
    local.get 11
    local.get 12
    i64.shr_s
    local.set 13
    local.get 13
    i32.wrap_i64
    local.set 14
    local.get 14
    return)
  (func $__original_main (type 1) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 131072
    local.set 4
    local.get 2
    local.get 4
    i32.store offset=8
    i32.const 196608
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
    call $FixedMul_int__int_
    local.set 8
    local.get 2
    local.get 8
    i32.store
    local.get 2
    i32.load
    local.set 9
    i32.const 16
    local.set 10
    local.get 9
    local.get 10
    i32.shr_s
    local.set 11
    i32.const 16
    local.set 12
    local.get 2
    local.get 12
    i32.add
    local.set 13
    local.get 13
    global.set $__stack_pointer
    local.get 11
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

// 15
inline constexpr std::string_view program20 = R"(
(module
  (type (;0;) (func (param i32) (result i32)))
  (type (;1;) (func (result i32)))
  (type (;2;) (func (param i32 i32) (result i32)))
  (func $sum_Tree*_ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 0
    local.set 5
    local.get 4
    local.set 6
    local.get 5
    local.set 7
    local.get 6
    local.get 7
    i32.ne
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
        br_if 0 (;@2;)
        i32.const 0
        local.set 11
        local.get 3
        local.get 11
        i32.store offset=12
        br 1 (;@1;)
      end
      local.get 3
      i32.load offset=8
      local.set 12
      local.get 12
      i32.load
      local.set 13
      local.get 3
      i32.load offset=8
      local.set 14
      local.get 14
      i32.load offset=4
      local.set 15
      local.get 15
      call $sum_Tree*_
      local.set 16
      local.get 13
      local.get 16
      i32.add
      local.set 17
      local.get 3
      i32.load offset=8
      local.set 18
      local.get 18
      i32.load offset=8
      local.set 19
      local.get 19
      call $sum_Tree*_
      local.set 20
      local.get 17
      local.get 20
      i32.add
      local.set 21
      local.get 3
      local.get 21
      i32.store offset=12
    end
    local.get 3
    i32.load offset=12
    local.set 22
    i32.const 16
    local.set 23
    local.get 3
    local.get 23
    i32.add
    local.set 24
    local.get 24
    global.set $__stack_pointer
    local.get 22
    return)
  (func $__original_main (type 1) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    local.set 0
    i32.const 80
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
    i32.store offset=76
    i32.const 64
    local.set 4
    local.get 2
    local.get 4
    i32.add
    local.set 5
    local.get 5
    local.set 6
    i32.const 1
    local.set 7
    local.get 6
    local.get 7
    call $Tree::Tree_int_
    drop
    i32.const 48
    local.set 8
    local.get 2
    local.get 8
    i32.add
    local.set 9
    local.get 9
    local.set 10
    i32.const 2
    local.set 11
    local.get 10
    local.get 11
    call $Tree::Tree_int_
    drop
    i32.const 32
    local.set 12
    local.get 2
    local.get 12
    i32.add
    local.set 13
    local.get 13
    local.set 14
    i32.const 3
    local.set 15
    local.get 14
    local.get 15
    call $Tree::Tree_int_
    drop
    i32.const 16
    local.set 16
    local.get 2
    local.get 16
    i32.add
    local.set 17
    local.get 17
    local.set 18
    i32.const 4
    local.set 19
    local.get 18
    local.get 19
    call $Tree::Tree_int_
    drop
    local.get 2
    local.set 20
    i32.const 5
    local.set 21
    local.get 20
    local.get 21
    call $Tree::Tree_int_
    drop
    i32.const 48
    local.set 22
    local.get 2
    local.get 22
    i32.add
    local.set 23
    local.get 23
    local.set 24
    local.get 2
    local.get 24
    i32.store offset=68
    i32.const 32
    local.set 25
    local.get 2
    local.get 25
    i32.add
    local.set 26
    local.get 26
    local.set 27
    local.get 2
    local.get 27
    i32.store offset=72
    i32.const 16
    local.set 28
    local.get 2
    local.get 28
    i32.add
    local.set 29
    local.get 29
    local.set 30
    local.get 2
    local.get 30
    i32.store offset=52
    local.get 2
    local.set 31
    local.get 2
    local.get 31
    i32.store offset=56
    i32.const 64
    local.set 32
    local.get 2
    local.get 32
    i32.add
    local.set 33
    local.get 33
    local.set 34
    local.get 34
    call $sum_Tree*_
    local.set 35
    i32.const 80
    local.set 36
    local.get 2
    local.get 36
    i32.add
    local.set 37
    local.get 37
    global.set $__stack_pointer
    local.get 35
    return)
  (func $Tree::Tree_int_ (type 2) (param i32 i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32)
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
    i32.store
    i32.const 0
    local.set 7
    local.get 5
    local.get 7
    i32.store offset=4
    i32.const 0
    local.set 8
    local.get 5
    local.get 8
    i32.store offset=8
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

// 8
inline constexpr std::string_view program21 = R"(
(module
  (type (;0;) (func (param i32) (result i32)))
  (type (;1;) (func (result i32)))
  (type (;2;) (func (param i32 i32) (result i32)))
  (func $lookup_sin_int_ (type 0) (param i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    i32.const 8
    local.set 5
    local.get 4
    local.get 5
    i32.rem_s
    local.set 6
    i32.const 1024
    local.set 7
    i32.const 2
    local.set 8
    local.get 6
    local.get 8
    i32.shl
    local.set 9
    local.get 7
    local.get 9
    i32.add
    local.set 10
    local.get 10
    i32.load
    local.set 11
    local.get 11
    return)
  (func $__original_main (type 1) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
        i32.const 8
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
        local.get 2
        i32.load offset=4
        local.set 13
        local.get 13
        call $lookup_sin_int_
        local.set 14
        local.get 2
        i32.load offset=8
        local.set 15
        local.get 15
        local.get 14
        i32.add
        local.set 16
        local.get 2
        local.get 16
        i32.store offset=8
        local.get 2
        i32.load offset=4
        local.set 17
        i32.const 1
        local.set 18
        local.get 17
        local.get 18
        i32.add
        local.set 19
        local.get 2
        local.get 19
        i32.store offset=4
        br 0 (;@2;)
      end
    end
    local.get 2
    i32.load offset=8
    local.set 20
    i32.const 16
    local.set 21
    local.get 2
    local.get 21
    i32.add
    local.set 22
    local.get 22
    global.set $__stack_pointer
    local.get 20
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
  (data $.rodata (i32.const 1024) "\00\00\00\00\01\00\00\00\02\00\00\00\03\00\00\00\02\00\00\00\01\00\00\00\00\00\00\00\ff\ff\ff\ff"))
)";

// 2016
inline constexpr std::string_view program = R"(
(module
  (type (;0;) (func (param i32)))
  (type (;1;) (func (result i32)))
  (type (;2;) (func (param i32 i32) (result i32)))
  (func $fill_int_ (type 0) (param i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
        i32.load offset=8
        local.set 5
        i32.const 8
        local.set 6
        local.get 5
        local.set 7
        local.get 6
        local.set 8
        local.get 7
        local.get 8
        i32.lt_s
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
        i32.const 0
        local.set 12
        local.get 3
        local.get 12
        i32.store offset=4
        block  ;; label = @3
          loop  ;; label = @4
            local.get 3
            i32.load offset=4
            local.set 13
            i32.const 8
            local.set 14
            local.get 13
            local.set 15
            local.get 14
            local.set 16
            local.get 15
            local.get 16
            i32.lt_s
            local.set 17
            i32.const 1
            local.set 18
            local.get 17
            local.get 18
            i32.and
            local.set 19
            local.get 19
            i32.eqz
            br_if 1 (;@3;)
            local.get 3
            i32.load offset=12
            local.set 20
            local.get 3
            i32.load offset=8
            local.set 21
            i32.const 3
            local.set 22
            local.get 21
            local.get 22
            i32.shl
            local.set 23
            local.get 20
            local.get 23
            i32.add
            local.set 24
            local.get 3
            i32.load offset=4
            local.set 25
            local.get 24
            local.get 25
            i32.add
            local.set 26
            local.get 3
            i32.load offset=8
            local.set 27
            i32.const 1024
            local.set 28
            i32.const 5
            local.set 29
            local.get 27
            local.get 29
            i32.shl
            local.set 30
            local.get 28
            local.get 30
            i32.add
            local.set 31
            local.get 3
            i32.load offset=4
            local.set 32
            i32.const 2
            local.set 33
            local.get 32
            local.get 33
            i32.shl
            local.set 34
            local.get 31
            local.get 34
            i32.add
            local.set 35
            local.get 35
            local.get 26
            i32.store
            local.get 3
            i32.load offset=4
            local.set 36
            i32.const 1
            local.set 37
            local.get 36
            local.get 37
            i32.add
            local.set 38
            local.get 3
            local.get 38
            i32.store offset=4
            br 0 (;@4;)
          end
        end
        local.get 3
        i32.load offset=8
        local.set 39
        i32.const 1
        local.set 40
        local.get 39
        local.get 40
        i32.add
        local.set 41
        local.get 3
        local.get 41
        i32.store offset=8
        br 0 (;@2;)
      end
    end
    return)
  (func $sum__ (type 1) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32 i32)
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
    block  ;; label = @1
      loop  ;; label = @2
        local.get 2
        i32.load offset=8
        local.set 5
        i32.const 8
        local.set 6
        local.get 5
        local.set 7
        local.get 6
        local.set 8
        local.get 7
        local.get 8
        i32.lt_s
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
        i32.const 0
        local.set 12
        local.get 2
        local.get 12
        i32.store offset=4
        block  ;; label = @3
          loop  ;; label = @4
            local.get 2
            i32.load offset=4
            local.set 13
            i32.const 8
            local.set 14
            local.get 13
            local.set 15
            local.get 14
            local.set 16
            local.get 15
            local.get 16
            i32.lt_s
            local.set 17
            i32.const 1
            local.set 18
            local.get 17
            local.get 18
            i32.and
            local.set 19
            local.get 19
            i32.eqz
            br_if 1 (;@3;)
            local.get 2
            i32.load offset=8
            local.set 20
            i32.const 1024
            local.set 21
            i32.const 5
            local.set 22
            local.get 20
            local.get 22
            i32.shl
            local.set 23
            local.get 21
            local.get 23
            i32.add
            local.set 24
            local.get 2
            i32.load offset=4
            local.set 25
            i32.const 2
            local.set 26
            local.get 25
            local.get 26
            i32.shl
            local.set 27
            local.get 24
            local.get 27
            i32.add
            local.set 28
            local.get 28
            i32.load
            local.set 29
            local.get 2
            i32.load offset=12
            local.set 30
            local.get 30
            local.get 29
            i32.add
            local.set 31
            local.get 2
            local.get 31
            i32.store offset=12
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
            br 0 (;@4;)
          end
        end
        local.get 2
        i32.load offset=8
        local.set 35
        i32.const 1
        local.set 36
        local.get 35
        local.get 36
        i32.add
        local.set 37
        local.get 2
        local.get 37
        i32.store offset=8
        br 0 (;@2;)
      end
    end
    local.get 2
    i32.load offset=12
    local.set 38
    local.get 38
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
    i32.const 0
    local.set 4
    local.get 4
    call $fill_int_
    call $sum__
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
  (global $__stack_pointer (mut i32) (i32.const 66816))
  (export "memory" (memory 0))
  (export "main" (func $main)))
)";