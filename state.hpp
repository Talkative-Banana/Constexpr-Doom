#pragma once
#include "./test/program.hpp"
#include "types.hpp"
#include <array>
#include <string_view>
#include <bit>

struct L1Cache {};
struct L2Cache {};


struct Data {
  std::string_view m_strId{};
  uint64_t m_data{};

  template <typename T> constexpr T get() const {
    if constexpr (std::is_same_v<T, int32_t>)
      return static_cast<int32_t>(m_data);
    else if constexpr (std::is_same_v<T, uint32_t>)
      return static_cast<uint32_t>(m_data);
    else if constexpr (std::is_same_v<T, int64_t>)
      return static_cast<int64_t>(m_data);
    else if constexpr (std::is_same_v<T, uint64_t>)
      return m_data;
    else if constexpr (std::is_same_v<T, float>)
      return std::bit_cast<float>(static_cast<uint32_t>(m_data));
    else if constexpr (std::is_same_v<T, double>)
      return std::bit_cast<double>(m_data);
    else
      static_assert(sizeof(T) == 0, "Unsupported type in Data::get");
  }

  constexpr void set(int32_t v) {
    m_data = static_cast<uint64_t>(static_cast<uint32_t>(v));
  }
  constexpr void set(uint32_t v) { m_data = static_cast<uint64_t>(v); }
  constexpr void set(int64_t v) { m_data = static_cast<uint64_t>(v); }
  constexpr void set(uint64_t v) { m_data = v; }
  constexpr void set(float v) { m_data = std::bit_cast<uint32_t>(v); }
  constexpr void set(double v) { m_data = std::bit_cast<uint64_t>(v); }
};

constexpr double ParseHexDecimal(std::string_view operand) {
  std::string_view s = operand;
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
  operand.remove_prefix(2); // skip "0x"

  double mantissa = 0.0f;
  double frac = 1.0f;
  bool hasDot = false;
  size_t i = 0;

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

  i++; // skip 'p'
  int expSign = 1;
  if (i < operand.size() && operand[i] == '+') {
    i++;
  } else if (i < operand.size() && operand[i] == '-') {
    expSign = -1;
    i++;
  }

  int exp = 0;
  for (; i < operand.size(); i++)
    exp = exp * 10 + (operand[i] - '0');
  exp *= expSign;

  double result = mantissa;
  if (exp > 0) {
    for (int j = 0; j < exp; j++)
      result *= 2.0f;
  } else {
    for (int j = 0; j < -exp; j++)
      result /= 2.0f;
  }
  return neg ? -result : result;
}

struct BrTablePool {
  std::array<uint32_t, BRTABLESIZE * 64>
      m_data{}; // 512*64 = 32768 entries = 128 KB
  uint32_t m_ptr = 0;

  constexpr uint32_t Alloc(uint32_t count) {
    uint32_t offset = m_ptr;
    m_ptr += count;
    if (m_ptr > m_data.size())
      throw "BrTablePool overflow";
    return offset;
  }
};

struct Instr {
  OP m_op{};
  Member m_mem{};
  OperandType m_type{};
  std::string_view m_operand{};
  int64_t m_operandValue{};
  double m_operandValueDecimal{};
  int32_t m_brCount{};
  int32_t m_brTableOffset{}; // index into BrTablePool, -1 if unused

  constexpr Instr() = default;

  constexpr Instr(OP op, Member mem, OperandType type, std::string_view operand,
                  int64_t operandValue = 0)
      : m_op(op), m_mem(mem), m_type(type), m_operand(operand),
        m_operandValue(operandValue), m_operandValueDecimal(0.0), m_brCount(0),
        m_brTableOffset(-1) {}

  // Main parsing constructor — does NOT handle br_table entries.
  // br_table entries are written into BrTablePool by Function::AddInstr().
  constexpr Instr(std::string_view op, std::string_view operand)
      : m_operand(operand), m_brTableOffset(-1) {

    size_t pos = op.find(".");
    std::string_view _op = op.substr(0, pos);
    std::string_view _mem;
    if (pos != std::string_view::npos)
      _mem = op.substr(pos + 1);

    // ── set OP ──
    if (_op == "local")
      m_op = OP::_local;
    else if (_op == "global")
      m_op = OP::_global;
    else if (_op == "i32")
      m_op = OP::_i32;
    else if (_op == "i64")
      m_op = OP::_i64;
    else if (_op == "f32")
      m_op = OP::_f32;
    else if (_op == "f64")
      m_op = OP::_f64;
    else if (_op == "return")
      m_op = OP::_return;
    else if (_op == "call")
      m_op = OP::_call;
    else if (_op == "halt")
      m_op = OP::_unreachable;
    else if (_op == "unreachable")
      m_op = OP::_unreachable;
    else if (_op == "block")
      m_op = OP::_block;
    else if (_op == "loop")
      m_op = OP::_loop;
    else if (_op == "br_if")
      m_op = OP::_br_if;
    else if (_op == "br")
      m_op = OP::_br;
    else if (_op == "end")
      m_op = OP::_end;
    else if (_op == "call_indirect")
      m_op = OP::_call_indirect;
    else if (_op == "drop")
      m_op = OP::_drop;
    else if (_op == "select")
      m_op = OP::_select;
    else if (_op == "br_table")
      m_op = OP::_br_table;
    else
      throw "Opcode not supported";

    // block/loop/select have no member or immediate
    if (m_op == OP::_block || m_op == OP::_loop || m_op == OP::_select)
      return;

    // br_table: entries are parsed later by Function::AddInstr — just return
    if (m_op == OP::_br_table)
      return;

    bool is_LoadStore = false;

    // ── set Member ──
    if (pos == std::string_view::npos) {
      m_mem = Member::_none;
    } else if (_mem == "get")
      m_mem = Member::_get;
    else if (_mem == "set")
      m_mem = Member::_set;
    else if (_mem == "tee")
      m_mem = Member::_tee;
    else if (_mem == "const")
      m_mem = Member::_const;
    else if (_mem == "add")
      m_mem = Member::_add;
    else if (_mem == "sub")
      m_mem = Member::_sub;
    else if (_mem == "mul")
      m_mem = Member::_mul;
    else if (_mem == "store")
      m_mem = Member::_store;
    else if (_mem == "load")
      m_mem = Member::_load;
    else if (_mem == "le_s")
      m_mem = Member::_le_s;
    else if (_mem == "le_u")
      m_mem = Member::_le_u;
    else if (_mem == "lt_s")
      m_mem = Member::_lt_s;
    else if (_mem == "lt_u")
      m_mem = Member::_lt_u;
    else if (_mem == "and")
      m_mem = Member::_and;
    else if (_mem == "eqz")
      m_mem = Member::_eqz;
    else if (_mem == "eq")
      m_mem = Member::_eq;
    else if (_mem == "xor")
      m_mem = Member::_xor;
    else if (_mem == "or")
      m_mem = Member::_or;
    else if (_mem == "rem_s")
      m_mem = Member::_rem_s;
    else if (_mem == "rem_u")
      m_mem = Member::_rem_u;
    else if (_mem == "shl")
      m_mem = Member::_shl;
    else if (_mem == "div_s")
      m_mem = Member::_div_s;
    else if (_mem == "div_u")
      m_mem = Member::_div_u;
    else if (_mem == "ne")
      m_mem = Member::_ne;
    else if (_mem == "shr_s")
      m_mem = Member::_shr_s;
    else if (_mem == "shr_u")
      m_mem = Member::_shr_u;
    else if (_mem == "load8_u") {
      m_mem = Member::_load8_u;
      is_LoadStore = true;
    } else if (_mem == "load8_s") {
      m_mem = Member::_load8_s;
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
    } else if (_mem == "gt_s")
      m_mem = Member::_gt_s;
    else if (_mem == "gt_u")
      m_mem = Member::_gt_u;
    else if (_mem == "abs")
      m_mem = Member::_abs;
    else if (_mem == "lt")
      m_mem = Member::_lt;
    else if (_mem == "trunc_f32_s")
      m_mem = Member::_trunc_f32_s;
    else if (_mem == "trunc_f64_s")
      m_mem = Member::_trunc_f64_s;
    else if (_mem == "convert_i32_s")
      m_mem = Member::_convert_i32_s;
    else if (_mem == "convert_i64_s")
      m_mem = Member::_convert_i64_s;
    else if (_mem == "promote_f32")
      m_mem = Member::_promote_f32;
    else if (_mem == "demote_f64")
      m_mem = Member::_demote_f64;
    else if (_mem == "extend_i32_s")
      m_mem = Member::_extend_i32_s;
    else if (_mem == "wrap_i64")
      m_mem = Member::_wrap_i64;
    else if (_mem == "ge_s")
      m_mem = Member::_ge_s;
    else if (_mem == "ge_u")
      m_mem = Member::_ge_u;
    else if (_mem == "ge")
      m_mem = Member::_ge;
    else if (_mem == "div")
      m_mem = Member::_div;
    else
      throw "Invalid Member Parsing";

    bool isInt = (m_op == OP::_i32 || m_op == OP::_i64);
    bool isDec = (m_op == OP::_f32 || m_op == OP::_f64);
    bool isIImm = (isInt && m_mem == Member::_const);
    bool isDImm = (isDec && m_mem == Member::_const);

    // ── parse immediate ──
    if (isIImm || m_op == OP::_local || m_op == OP::_global ||
        m_op == OP::_call) {
      int64_t value = 0;
      size_t i = 0;
      bool isNeg = false;
      while (i < m_operand.size() &&
             (m_operand[i] == '-' ||
              (m_operand[i] >= '0' && m_operand[i] <= '9'))) {
        if (m_operand[i] == '-') {
          isNeg = true;
          i++;
          continue;
        }
        value = value * 10 + (m_operand[i++] - '0');
      }
      m_operandValue = isNeg ? -value : value;
    } else if (isDImm) {
      m_operandValueDecimal = ParseHexDecimal(operand);
    } else if (m_operand.empty()) {
      m_type = OperandType::_none;
    } else if (m_operand[0] >= '0' && m_operand[0] <= '9') {
      m_type = OperandType::_immediate;
      int64_t value = 0;
      size_t i = 0;
      bool isNeg = false;
      while (i < m_operand.size() &&
             (m_operand[i] == '-' ||
              (m_operand[i] >= '0' && m_operand[i] <= '9'))) {
        if (m_operand[i] == '-') {
          isNeg = true;
          i++;
          continue;
        }
        value = value * 10 + (m_operand[i++] - '0');
      }
      m_operandValue = isNeg ? -value : value;
    } else if (m_operand[0] == '$') {
      m_type = OperandType::_address;
    } else if (m_operand[0] == 'o') {
      m_type = OperandType::_offset;
      int64_t value = 0;
      size_t i = 7; // skip "offset="
      bool isNeg = false;
      while (i < m_operand.size() &&
             (m_operand[i] == '-' ||
              (m_operand[i] >= '0' && m_operand[i] <= '9'))) {
        if (m_operand[i] == '-') {
          isNeg = true;
          i++;
          continue;
        }
        value = value * 10 + (m_operand[i++] - '0');
      }
      m_operandValue = isNeg ? -value : value;
    } else if (m_op == OP::_call_indirect) {
      return; // skip type check
    } else if (is_LoadStore) {
      m_operandValue = 0;
    }
  }
};

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

  std::array<ParamType, 8192> m_locals{};
  uint32_t m_localCount = 0;

  std::array<Instr, 8192> m_body{};
  uint32_t m_bodyCount = 0;

  std::array<uint32_t, BLOCKSTACKSIZE> m_blockStack{};
  std::array<Block, BLOCKSIZE> m_blockTable{};
  uint32_t m_blockStackPointer = BLOCKSTACKSIZE;
  uint32_t m_blockIdx = 0;
  bool m_isDefined{false};

  constexpr Block &getBlock() { return m_blockTable[m_blockIdx++]; }

  constexpr void Push(uint32_t idx) {
    if (m_blockStackPointer == 0)
      throw "Block stack overflow";
    m_blockStack[--m_blockStackPointer] = idx;
  }
  constexpr Block &Pop() {
    if (m_blockStackPointer == BLOCKSTACKSIZE)
      throw "Block stack underflow";
    return m_blockTable[m_blockStack[m_blockStackPointer++]];
  }

  // Call this instead of writing to m_body directly.
  // Parses br_table entries into the global pool.
  constexpr void AddInstr(Instr instr, BrTablePool &pool) {
    if (instr.m_op == OP::_br_table) {
      instr.m_brTableOffset = static_cast<int32_t>(pool.m_ptr);
      instr.m_brCount = 0;

      std::string_view s = instr.m_operand;
      size_t pos = 0;
      while (pos < s.size()) {
        // skip whitespace
        while (pos < s.size() && s[pos] == ' ')
          pos++;
        if (pos >= s.size())
          break;

        // skip (;...;) comments
        if (s[pos] == '(') {
          size_t end = s.find(";)", pos);
          pos = (end != std::string_view::npos) ? end + 2 : pos + 1;
          continue;
        }

        // parse number
        if (s[pos] >= '0' && s[pos] <= '9') {
          uint32_t value = 0;
          while (pos < s.size() && s[pos] >= '0' && s[pos] <= '9')
            value = value * 10 + (s[pos++] - '0');
          pool.m_data[pool.m_ptr++] = value;
          instr.m_brCount++;
          if (pool.m_ptr > pool.m_data.size())
            throw "BrTablePool overflow";
        } else {
          pos++;
        }
      }

      if (instr.m_brCount == 0)
        throw "br_table has no entries";
    }

    if (m_bodyCount >= m_body.size())
      throw "Function body overflow";
    m_body[m_bodyCount++] = instr;
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
  constexpr void Push(const Data &data) {
    if (m_stackPointer == STACKSIZE)
      throw "Stack overflow";
    m_vargCount++;
    m_data[++m_stackPointer] = data;
  }
  constexpr Data Pop() {
    if (m_stackPointer == m_floorPointer)
      throw "Stack underflow";
    m_vargCount--;
    return m_data[m_stackPointer--];
  }

  uint64_t m_basePointer = 0;
  uint64_t m_stackPointer = 0;
  uint64_t m_framePointer = 0;
  uint64_t m_floorPointer = 0;
  uint64_t m_vargCount = 0;
  std::array<Data, STACKSIZE> m_data{};
};

struct GarbageCollector {
  uint32_t m_gcPtr = 0;
  std::array<uint32_t, GCSIZE> m_data{};
};

struct Heap {
  uint32_t m_heapPtr = 0;
  GarbageCollector m_garbageCollector{};
};

struct FrameBuffer {
  uint32_t m_framePtr = 0;
  std::array<char, SCREENWIDTH * SCREENHEIGHT> m_data{};
};

struct FileDesc {
  bool m_open = false;
  uint32_t m_offset = 0;
  uint32_t m_size = 0;
  uint32_t m_dataPtr = 0;
};

struct Descriptor {
  std::array<FileDesc, FDTABLE> m_fdTable{};
};

struct FileSystem {
  std::array<char, FILESYSTEMSIZE> m_data{};
};

struct Color { uint8_t r, g, b; };

struct Palette {
  std::array<Color, 256> m_data{};
};

struct State {
  Heap m_heap{};
  Stack m_stack{};
  Stack m_opStack{};
  Global m_global{};
  Memory m_memory{};
  Descriptor m_descriptor{};
  FileSystem m_fileSystem{};
  FrameBuffer m_frameBuffer{};
  uint64_t m_instrPointer = 0;
  BrTablePool m_brTablePool{};
  FunctionTable m_functionTable{};
  VirtualTable m_virtualTable{};
  Function *m_activeFunction = nullptr;
  Palette m_palette{};
  uint64_t m_ticks = 0;
};

constexpr std::size_t getFunctionId(State &state, std::string_view str) {
  FunctionTable &ft = state.m_functionTable;
  for (int i = 0; i < ft.m_count; i++)
    if (ft.m_data[i].m_name == str)
      return i;
  return 0;
}

constexpr std::size_t getOperandId(State &state, std::string_view str) {
  Global &g = state.m_global;
  for (int i = 0; i < g.m_count; i++)
    if (g.m_data[i].m_strId == str)
      return i;
  return 0;
}

constexpr uint32_t brTableJump(State &state, const Instr &instr, uint32_t idx) {
  if (instr.m_brCount == 0)
    throw "br_table: no entries";
  if (idx >= static_cast<uint32_t>(instr.m_brCount))
    idx = static_cast<uint32_t>(instr.m_brCount) - 1; // default case
  return state.m_brTablePool.m_data[instr.m_brTableOffset + idx];
}