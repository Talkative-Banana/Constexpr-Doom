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
  ISBAD,
  ERROR,
  ERROR_MEMCPY,
  ERROR_FSEEK,
  ERROR_FTELL,
  ERROR_FREAD,
  ERROR_FCLOSE,
  ERROR_EXIT,
  ERROR_MKDIR,
  ERROR_ATOI,
  ERROR_GETCHAR,
  ERROR_FPRINTF,
  ERROR_FPUTC,
  ERROR_FWRITE,
  ERROR_STRCMP,
  ERROR_WRITE,
  ERROR_FEOF,
  ERROR_FSCANF,
  ERROR_SSCANF,
  SYSFUNCERROR,
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
  _rem_u,
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
  _load8_s,
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
