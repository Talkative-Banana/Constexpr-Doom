#pragma once
#include <array>
#include <assert.h>
#include <cstdint>
#include <string_view>
#include <type_traits>
#include <variant>

struct Span {
  uint32_t begin, end;
};

// ----- OP TYPES -----
enum class WASMOP {
  _unknown,
  _func,
  _table,
  _type,
  _memory,
  _global,
  _import,
  _export,
  _data,
  _elem,
};

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
  } else if (block.starts_with("(data")) {
    return WASMOP::_data;
  } else if (block.starts_with("(elem")) {
    return WASMOP::_elem;
  } else if (block.starts_with("(import")) {
    return WASMOP::_import;
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
  _end,
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

enum class ParamType {
  _i32,
  _i64,
  _f32,
  _f64,
  _funcref,
  _externref,
};

enum class STATUS {
  OK,
  ERROR,
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
  _div_s,
  _div_u,
  _rem_s,
  _and,
  _or,
  _xor,
  _shl,
  _shr_s,
  _shr_u,
  _rotl,
  _rotr,
  _div,
  _min,
  _max,
  _copysign,
  _eqz,
  _ne,
  _lt_s,
  _lt_u,
  _gt_s,
  _gt_u,
  _le_s,
  _le_u,
  _ge_s,
  _ge_u,
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
  _promote_f32,
  _demote_f64,
  _demote,
  _store,
  _load,
  _load8_u,
  _load16_u,
  _load16_s,
  _store8,
  _store16,
  _trunc_f32_s,
  _trunc_f64_s,
  _convert_i32_s,
  _convert_i64_s,
  _extend_i32_s,
  _wrap_i64,
};

enum class OperandType {
  _none,
  _immediate,
  _address,
  _offset,
  _hash,
};

enum Trap { terminate };

template <typename Expr, typename T1, typename T2> struct SELECT {
  using type = typename std::conditional<Expr::value, T1, T2>::type;
};

struct Data {
  std::variant<int32_t, uint32_t, int64_t, uint64_t, float, double> m_data{};
};

constexpr double ParseHexDecimal(std::string_view operand) {
  std::string_view s = operand;
  // strip (;...;) comment
  size_t commentPos = s.find("(;");
  if (commentPos != std::string_view::npos) {
    s = s.substr(0, commentPos);
    while (!s.empty() && s.back() == ' ')
      s.remove_suffix(1);
  }

  operand = s;
  bool neg = false;
  if (!operand.empty() && operand[0] == '-') {
    neg = true;
    operand.remove_prefix(1);
  }
  // skip "0x"
  operand.remove_prefix(2);

  double mantissa = 0.0f;
  double frac = 1.0f;
  bool hasDot = false;
  size_t i = 0;

  // parse hex mantissa
  for (; i < operand.size(); i++) {
    char c = operand[i];
    if (c == 'p' || c == 'P')
      break;
    if (c == '.') {
      hasDot = true;
      continue;
    }
    int digit = (c >= '0' && c <= '9')   ? c - '0'
                : (c >= 'a' && c <= 'f') ? c - 'a' + 10
                                         : c - 'A' + 10;
    if (hasDot) {
      frac /= 16.0f;
      mantissa += digit * frac;
    } else {
      mantissa = mantissa * 16.0f + digit;
    }
  }

  // parse exponent after 'p'
  i++; // skip 'p'
  int expSign = 1;
  if (operand[i] == '+') {
    i++;
  } else if (operand[i] == '-') {
    expSign = -1;
    i++;
  }

  int exp = 0;
  for (; i < operand.size(); i++) {
    exp = exp * 10 + (operand[i] - '0');
  }
  exp *= expSign;

  // apply exponent (2^exp)
  double result = mantissa;
  if (exp > 0) {
    for (int j = 0; j < exp; j++) {
      result *= 2.0f;
    }
  } else {
    for (int j = 0; j < -exp; j++) {
      result /= 2.0f;
    }
  }
  return neg ? -result : result;
}

struct Instr {
  OP m_op{};
  Member m_mem{};
  OperandType m_type{};
  std::string_view m_operand{};
  int64_t m_operandValue{};
  double m_operandValueDecimal{};
  int32_t m_brCount{};
  std::array<uint32_t, BRTABLESIZE> m_brTable{};

  constexpr Instr() = default;
  constexpr Instr(OP op, Member mem, OperandType type, std::string_view operand,
                  int64_t operandValue = 0)
      : m_op(op), m_mem(mem), m_type(type), m_operand(operand),
        m_operandValue(operandValue), m_operandValueDecimal(0.0) {}
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
    } else if (_op == "i64") {
      m_op = OP::_i64;
    } else if (_op == "f32") {
      m_op = OP::_f32;
    } else if (_op == "f64") {
      m_op = OP::_f64;
    } else if (_op == "return") {
      m_op = OP::_return;
    } else if (_op == "call") {
      m_op = OP::_call;
    } else if (_op == "halt") {
      m_op = OP::_unreachable;
    } else if (_op == "unreachable") {
      m_op = OP::_unreachable;
    } else if (_op == "block") {
      m_op = OP::_block;
    } else if (_op == "loop") {
      m_op = OP::_loop;
    } else if (_op == "br_if") {
      m_op = OP::_br_if;
    } else if (_op == "br") {
      m_op = OP::_br;
    } else if (_op == "end") {
      m_op = OP::_end;
    } else if (_op == "call_indirect") {
      m_op = OP::_call_indirect;
    } else if (_op == "drop") {
      m_op = OP::_drop;
    } else if (_op == "select") {
      m_op = OP::_select;
    } else if (_op == "br_table") {
      m_op = OP::_br_table;
    } else {
      throw "Local instructions not supported yet";
    }

    // Early return in case of block elements
    if (m_op == OP::_block || m_op == OP::_loop || m_op == OP::_select) {
      return;
    }

    bool is_LoadStore = false;
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
    } else if (_mem == "le_s") {
      m_mem = Member::_le_s;
    } else if (_mem == "le_u") {
      m_mem = Member::_le_u;
    } else if (_mem == "lt_s") {
      m_mem = Member::_lt_s;
    } else if (_mem == "lt_u") {
      m_mem = Member::_lt_u;
    } else if (_mem == "and") {
      m_mem = Member::_and;
    } else if (_mem == "eqz") {
      m_mem = Member::_eqz;
    } else if (_mem == "eq") {
      m_mem = Member::_eq;
    } else if (_mem == "xor") {
      m_mem = Member::_xor;
    } else if (_mem == "or") {
      m_mem = Member::_or;
    } else if (_mem == "rem_s") {
      m_mem = Member::_rem_s;
    } else if (_mem == "shl") {
      m_mem = Member::_shl;
    } else if (_mem == "div_s") {
      m_mem = Member::_div_s;
    } else if (_mem == "div_u") {
      m_mem = Member::_div_u;
    } else if (_mem == "ne") {
      m_mem = Member::_ne;
    } else if (_mem == "shr_s") {
      m_mem = Member::_shr_s;
    } else if (_mem == "shr_u") {
      m_mem = Member::_shr_u;
    } else if (_mem == "load8_u") {
      m_mem = Member::_load8_u;
      is_LoadStore = true;
    } else if (_mem == "load16_u") {
      m_mem = Member::_load16_u;
      is_LoadStore = true;
    } else if (_mem == "load16_s") {
      m_mem = Member::_load16_s;
      is_LoadStore = true;
    } else if (_mem == "store8") {
      m_mem = Member::_store8;
      is_LoadStore = true;
    } else if (_mem == "store16") {
      m_mem = Member::_store16;
      is_LoadStore = true;
    } else if (_mem == "gt_s") {
      m_mem = Member::_gt_s;
    } else if (_mem == "gt_u") {
      m_mem = Member::_gt_u;
    } else if (_mem == "abs") {
      m_mem = Member::_abs;
    } else if (_mem == "lt") {
      m_mem = Member::_lt;
    } else if (_mem == "trunc_f32_s") {
      m_mem = Member::_trunc_f32_s;
    } else if (_mem == "trunc_f64_s") {
      m_mem = Member::_trunc_f64_s;
    } else if (_mem == "convert_i32_s") {
      m_mem = Member::_convert_i32_s;
    } else if (_mem == "convert_i64_s") {
      m_mem = Member::_convert_i64_s;
    } else if (_mem == "promote_f32") {
      m_mem = Member::_promote_f32;
    } else if (_mem == "demote_f64") {
      m_mem = Member::_demote_f64;
    } else if (_mem == "extend_i32_s") {
      m_mem = Member::_extend_i32_s;
    } else if (_mem == "wrap_i64") {
      m_mem = Member::_wrap_i64;
    } else if (_mem == "ge_s") {
      m_mem = Member::_ge_s;
    } else if (_mem == "ge_u") {
      m_mem = Member::_ge_u;
    } else if (_mem == "ge") {
      m_mem = Member::_ge;
    } else if (_mem == "div") {
      m_mem = Member::_div;
    } else {
      throw "Invalid Memeber Parsing";
    }

    bool isInt = (m_op == OP::_i32 || m_op == OP::_i64);
    bool isDec = (m_op == OP::_f32 || m_op == OP::_f64);
    bool isIImm = (isInt && m_mem == Member::_const);
    bool isDImm = (isDec && m_mem == Member::_const);

    if (m_op == OP::_br_table) {
      std::string_view s = m_operand;
      m_brCount = 0;

      size_t pos = 0;
      while (pos < s.size()) {
        // skip whitespace
        while (pos < s.size() && s[pos] == ' ') {
          pos++;
        }

        if (pos >= s.size()) {
          break;
        }

        // skip (;...;) comment
        if (s[pos] == '(') {
          size_t end = s.find(";)", pos);
          if (end != std::string_view::npos) {
            pos = end + 2;
          } else {
            pos++;
          }
        }

        // parse number
        if (s[pos] >= '0' && s[pos] <= '9') {
          int32_t value = 0;
          while (pos < s.size() && s[pos] >= '0' && s[pos] <= '9') {
            value = value * 10 + (s[pos] - '0');
            pos++;
          }
          if (m_brCount >= BRTABLESIZE)
            throw "br_table overflow";
          m_brTable[m_brCount++] = value;
        } else {
          pos++;
        }
      }
      if (m_brCount == 0) {
        throw "br_table has no entries";
      }
    } else if (isIImm || m_op == OP::_local) {
      // Parse the operand as an immediate value for local and global
      // instructions
      int64_t value = 0;
      size_t type_pos = 0;
      bool isNeg = false;
      while (type_pos < m_operand.size() &&
             ((m_operand[type_pos] >= '0' && m_operand[type_pos] <= '9') ||
              m_operand[type_pos] == '-')) {
        if (m_operand[type_pos] == '-') {
          isNeg = true;
          type_pos++;
          continue;
        }
        value = value * 10 + (m_operand[type_pos] - '0');
        type_pos++;
      }
      if (isNeg) {
        m_operandValue = -value;
      } else {
        m_operandValue = value;
      }
    } else if (isDImm) {
      // Parse the operand as an immediate value for local and global
      // instructions
      m_operandValueDecimal = ParseHexDecimal(operand);
    } else if (m_operand.empty()) {
      // Set the operand type
      m_type = OperandType::_none;
    } else if (m_operand[0] >= '0' && m_operand[0] <= '9') {
      m_type = OperandType::_immediate;
      // Parse the immediate value
      int64_t value = 0;
      size_t type_pos = 0;
      bool isNeg = false;
      while (type_pos < m_operand.size() &&
             ((m_operand[type_pos] >= '0' && m_operand[type_pos] <= '9') ||
              m_operand[type_pos] == '-')) {
        if (m_operand[type_pos] == '-') {
          isNeg = true;
          type_pos++;
          continue;
        }
        value = value * 10 + (m_operand[type_pos] - '0');
        type_pos++;
      }
      if (isNeg) {
        m_operandValue = -value;
      } else {
        m_operandValue = value;
      }
    } else if (m_operand[0] == '$') {
      m_type = OperandType::_address;
    } else if (m_operand[0] == 'o') {
      // i32 store offset
      m_type = OperandType::_offset;
      // Parse the immediate value
      int64_t value = 0;
      size_t type_pos = 7; // skip the offset= part
      bool isNeg = false;
      while (type_pos < m_operand.size() &&
             ((m_operand[type_pos] >= '0' && m_operand[type_pos] <= '9') ||
              m_operand[type_pos] == '-')) {
        if (m_operand[type_pos] == '-') {
          isNeg = true;
          type_pos++;
          continue;
        }
        value = value * 10 + (m_operand[type_pos] - '0');
        type_pos++;
      }
      if (isNeg) {
        m_operandValue = -value;
      } else {
        m_operandValue = value;
      }
    } else if (m_op == OP::_call_indirect) {
      // TODO: skip type check for now
      return;
    } else if (is_LoadStore) {
      // no offset
      m_operandValue = 0; // default
    } else {
      OP op = m_op;
      if (op == OP::_local)
        throw "isLoadStore: op is local";
      if (op == OP::_global)
        throw "isLoadStore: op is global";
      if (op == OP::_return)
        throw "isLoadStore: op is return";
      if (op == OP::_call)
        throw "isLoadStore: op is call";
      if (op == OP::_unreachable)
        throw "isLoadStore: op is unreachable";
      if (op == OP::_block)
        throw "isLoadStore: op is block";
      if (op == OP::_loop)
        throw "isLoadStore: op is loop";
      if (op == OP::_br_if)
        throw "isLoadStore: op is br_if";
      if (op == OP::_br)
        throw "isLoadStore: op is br";
      if (op == OP::_end)
        throw "isLoadStore: op is end";
      if (op == OP::_call_indirect)
        throw "isLoadStore: op is call_indirect";
      if (op == OP::_drop)
        throw "isLoadStore: op is drop";
      if (op == OP::_select)
        throw "isLoadStore: op is select";
      if (op == OP::_br_table)
        throw "isLoadStore: op is br_table";
    }
  }
};