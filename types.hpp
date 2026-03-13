#pragma once
#include <assert.h>
#include <cstdint>
#include <string_view>
#include <type_traits>

struct Span {
  uint32_t begin, end;
};

// ----- OP TYPES -----
enum class WASMOP { _unknown, _func, _table, _type, _memory, _global, _export };

// ----- IDENTIFY FUNCTION -----
constexpr WASMOP Identify(std::string_view block) {
  if (block.starts_with("(func")) {
    return WASMOP::_func;
  } else if (block.starts_with("(type")) {
    return WASMOP::_type;
  } else if (block.starts_with("(table")) {
    return WASMOP::_table;
  } else if (block.starts_with("(memory")) {
    return WASMOP::_memory;
  } else if (block.starts_with("(global")) {
    return WASMOP::_global;
  } else if (block.starts_with("(export")) {
    return WASMOP::_export;
  } else {
    return WASMOP::_unknown;
  }
}

enum class OP {
  _nop,
  _drop,
  _select,
  _unreachable,
  _block,
  _loop,
  _if,
  _br,
  _br_if,
  _br_table,
  _br_on_null,
  _br_on_non_null,
  _br_on_cast,
  _br_on_cast_fail,
  _call,
  _call_ref,
  _call_indirect,
  _return,
  _return_call,
  _return_call_ref,
  _return_call_indirect,
  _throw,
  _throw_ref,
  _try_table,
  _catch,
  _catch_ref,
  _catch_all,
  _catch_all_ref,
  _local,
  _global,
  _table,
  _memarg,
  _loadop,
  _storeop,
  _vloadop,
  _numtype,
  _vectype,
  _memory,
  _data,
  _ref,
  _struct,
  _array,
  _i32,
  _i64,
  _f32,
  _f64,
};

enum class Member {
  _none,
  _get,
  _set,
  _tee,
  _size,
  _grow,
  _fill,
  _copy,
  _init,
  _drop,
  _func,
  _null,
  _is_null,
  _as_non_null,
  _eq,
  _test,
  _cast,
  _new,
  _new_default,
  _new_fixed,
  _get_sx,
  _new_data,
  _new_elem,
  _len,
  _init_data,
  _init_elem,
  _i,
  _convert_any,
  _convert_extern,
  _const,
  _clz,
  _ctz,
  _popcnt,
  _extend_sz_s,
  _abs,
  _neg,
  _sqrt,
  _ceil,
  _floor,
  _truncate,
  _nearest,
  _add,
  _sub,
  _mul,
  _div_sx,
  _rem_sx,
  _and,
  _or,
  _xor,
  _shl,
  _shr_sx,
  _rotl,
  _rotr,
  _div,
  _min,
  _max,
  _copysign,
  _eqz,
  _ne,
  _lt_sx,
  _gt_sx,
  _le_sx,
  _ge_sx,
  _lt,
  _gt,
  _le,
  _ge,
  _extend_sx,
  _wrap,
  _covert_sx,
  _reinterpret,
  _trunc_sx,
  _trunc_sat_sx,
  _promote,
  _demote,
  _store,
  _load,

};

enum class OperandType {
  _none,
  _immediate,
  _address,
  _offset,
};

enum Trap { terminate };

template <typename Expr, typename T1, typename T2> struct SELECT {
  using type = typename std::conditional<Expr::value, T1, T2>::type;
};

struct Instr {
  OP m_op{};
  Member m_mem{};
  OperandType m_type{};
  std::string_view m_operand;

  constexpr Instr() = default;
  constexpr Instr(std::string_view op, std::string_view operand)
      : m_operand(operand) {
    size_t pos = op.find(".");
    std::string_view _op = op.substr(0, pos);
    std::string_view _mem;
    if (pos != std::string_view::npos) {
      _mem = op.substr(pos + 1);
    }

    // Set the operation
    if (_op == "local") {
      m_op = OP::_local;
    } else if (_op == "global") {
      m_op = OP::_global;
    } else if (_op == "i32") {
      m_op = OP::_i32;
    } else if (_op == "return") {
      m_op = OP::_return;
    } else if (_op == "call") {
      m_op = OP::_call;
    } else {
      throw "Local instructions not supported yet";
    }

    // Set the member
    if (pos == std::string_view::npos) {
      // No member
      m_mem = Member::_none;
    } else if (_mem == "get") {
      m_mem = Member::_get;
    } else if (_mem == "set") {
      m_mem = Member::_set;
    } else if (_mem == "const") {
      m_mem = Member::_const;
    } else if (_mem == "add") {
      m_mem = Member::_add;
    } else if (_mem == "sub") {
      m_mem = Member::_sub;
    } else if (_mem == "sub") {
      m_mem = Member::_sub;
    } else if (_mem == "mul") {
      m_mem = Member::_mul;
    } else if (_mem == "store") {
      m_mem = Member::_store;
    } else if (_mem == "load") {
      m_mem = Member::_load;
    } else {
      throw "Invalid Memeber Parsing";
    }

    // Set the operand type
    if (m_operand.empty()) {
      m_type = OperandType::_none;
    } else if (m_operand[0] >= '0' && m_operand[0] <= '9') {
      m_type = OperandType::_immediate;
    } else if (m_operand[0] == '$') {
      m_type = OperandType::_address;
    } else if (m_operand[0] == 'o') {
      m_type = OperandType::_offset;
    } else {
      throw "Invalid Operand Parsing";
    }
  }
};
