#pragma once
#include "./test/program.hpp"
#include "types.hpp"
#include <array>
#include <string_view>
#include <variant>

struct L1Cache {};

struct L2Cache {};

struct Data {
  std::string_view m_strId{};
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
    } else if (_mem == "tee") {
      m_mem = Member::_tee;
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
    } else if (_mem == "rem_u") {
      m_mem = Member::_rem_u;
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
  std::array<char, SCREENWIDTH * SCREENHEIGHT> m_data;
};

struct FileDesc {
  bool m_open = false;
  uint32_t m_offset = 0;  // current read position
  uint32_t m_size = 0;    // file size
  uint32_t m_dataPtr = 0; // address in wasm memory where file bytes live
};

struct Descriptor {
  std::array<FileDesc, FDTABLE> m_fdTable{};
};

struct FileSystem {
  // For simplicity, we can pre-load all files into memory at fixed addresses,
  // and use the open syscall to get pointers to them. The file descriptor can
  // just be an index into a table that tracks which files are open.
  std::array<char, FILESYSTEMSIZE> m_data{};
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
  FunctionTable m_functionTable{};
  VirtualTable m_virtualTable{};
  Function *m_activeFunction = nullptr;
};

constexpr std::size_t getFunctionId(State &state, std::string_view str) {
  FunctionTable &funcTable = state.m_functionTable;

  for (int i = 0; i < funcTable.m_count; i++) {
    Function &f = funcTable.m_data[i];
    if (f.m_name == str) {
      return i;
    }
  }
  return 0;
}

constexpr std::size_t getOperandId(State &state, std::string_view str) {
  Global &global = state.m_global;

  for (int i = 0; i < global.m_count; i++) {
    Data &data = global.m_data[i];
    if (data.m_strId == str) {
      return i;
    }
  }
  return 0;
}