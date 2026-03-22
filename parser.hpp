#pragma once
#include "constants.hpp"
#include "handler.hpp"
#include "state.hpp"
#include "types.hpp"
#include <array>
#include <cstdint>
#include <string_view>

inline constexpr std::pair<uint32_t, std::array<Span, MAXMODULES>>
ParseProgram(std::string_view module) {
  std::array<Span, MAXMODULES> modules{};
  uint32_t start = 0, count = 0, open = 0;
  bool inString = false;

  for (uint32_t i = 0; i < module.size(); i++) {
    char c = module[i];

    if (c == '"') {
      inString = !inString;
      continue;
    }

    if (inString)
      continue;

    if (c == '(') {
      if (open == 0)
        start = i;
      open++;
    }
    if (c == ')') {
      open--;
      if (open == 0) {
        // Skip (module ..... )
        //         ^          ^
        //         |          |
        if (count >= MAXMODULES)
          throw "Too many modules";
        modules[count++] = {start + 7, i};
      }
    }
  }
  return {count, modules};
}

inline constexpr std::pair<uint32_t, std::array<Span, MAXMODULES>>
ParseProgramNoCheck(std::string_view module) {
  std::array<Span, MAXMODULES> modules{};
  uint32_t start = 0;

  for (uint32_t i = 0; i < module.size(); i++) {
    char c = module[i];
    if (c == '(') {
      start = i;
      break;
    }
  }

  for (uint32_t i = module.size() - 1; i-- > 0;) {
    char c = module[i];
    if (c == ')') {
      modules[0] = {start + 7, i};
      break;
    }
  }
  return {1, modules};
}

// ----- MODULE PARSER -----
inline constexpr uint32_t
ParseModuleItems(std::string_view module,
                 std::array<Span, MAXCHILDREN> &items) {
  uint32_t open = 0, start = 0, count = 0;
  bool inString = false;

  for (uint32_t i = 0; i < module.size(); ++i) {
    char c = module[i];

    if (c == '"') {
      inString = !inString;
      continue;
    }

    if (inString)
      continue;

    if (c == '(') {
      if (open == 0)
        start = i;
      open++;
    }
    if (c == ')') {
      if (open == 0)
        throw "Unbalanced parentheses";
      open--;
      if (open == 0 && count < MAXCHILDREN) {
        items[count++] = {start, i + 1};
      }
    }
  }
  return count;
}

// ----- MODULE VALIDATOR -----
inline constexpr uint32_t ParseModule(std::string_view module,
                                      std::array<Span, MAXCHILDREN> &items) {
  uint32_t count = ParseModuleItems(module, items);
  for (uint32_t i = 0; i < count; ++i) {
    Span span = items[i];
    std::string_view child = module.substr(span.begin, span.end - span.begin);
    WASMOP type = Identify(child);

    if (type == WASMOP::_unknown) {
      // throw if invalid child
      throw "Unknown module child while parsing!";
    }
  }

  return count;
}

constexpr std::pair<std::string_view, std::string_view>
split2(std::string_view s, char delimiter = ' ') {
  size_t pos = s.find(delimiter);
  if (pos == std::string_view::npos)
    return {s, {}};

  return {s.substr(0, pos), s.substr(pos + 1)};
}

constexpr Instr ParseInstruction(std::string_view line) {
  auto [op, rest] = split2(line);
  Instr instr{op, rest};
  return instr;
}

constexpr Function ParseFunction(std::string_view func) {
  Function f{};

  // ------------------------------------------------
  // FUNC NAME
  // ------------------------------------------------
  size_t pos = func.find("(func");
  pos += 5;

  while (pos < func.size() && func[pos] == ' ')
    pos++;

  size_t end = pos;
  while (end < func.size() && func[end] != ' ' && func[end] != '(')
    end++;

  f.m_name = func.substr(pos, end - pos);

  // ------------------------------------------------
  // TYPE INDEX
  // ------------------------------------------------
  auto type_pos = func.find("(type");
  if (type_pos != std::string_view::npos) {
    type_pos += 5;

    while (type_pos < func.size() && func[type_pos] == ' ')
      type_pos++;

    uint32_t value = 0;

    while (type_pos < func.size() && func[type_pos] >= '0' &&
           func[type_pos] <= '9') {
      value = value * 10 + (func[type_pos] - '0');
      type_pos++;
    }

    f.m_typeIndex = value;
  }

  // ------------------------------------------------
  // PARAMS
  // ------------------------------------------------
  size_t p = 0;

  while ((p = func.find("(param", p)) != std::string_view::npos) {
    p += 6;

    while (func[p] == ' ')
      p++;

    size_t close = func.find(')', p);

    while (p < close) {
      size_t space = func.find(' ', p);

      size_t e =
          (space == std::string_view::npos || space > close) ? close : space;

      if (f.m_paramCount >= f.m_params.size())
        throw "Too many params";

      auto tok = func.substr(p, e - p);

      if (!tok.empty() && tok[0] != '$') {
        if (tok == "i32") {
          f.m_params[f.m_paramCount++] = ParamType::_i32;
        } else if (tok == "i64") {
          f.m_params[f.m_paramCount++] = ParamType::_i64;
        } else if (tok == "f32") {
          f.m_params[f.m_paramCount++] = ParamType::_f32;
        } else if (tok == "f64") {
          f.m_params[f.m_paramCount++] = ParamType::_f64;
        } else {
          throw "Invalid param type";
        }
      }

      p = e + 1;
    }

    p = close + 1;
  }

  // ------------------------------------------------
  // LOCALS
  // ------------------------------------------------
  p = 0;

  while ((p = func.find("(local", p)) != std::string_view::npos) {
    p += 6;

    while (func[p] == ' ')
      p++;

    size_t close = func.find(')', p);

    while (p < close) {
      size_t space = func.find(' ', p);

      size_t e =
          (space == std::string_view::npos || space > close) ? close : space;

      if (f.m_localCount >= f.m_locals.size())
        throw "Too many locals";

      auto tok = func.substr(p, e - p);

      if (!tok.empty() && tok[0] != '$') {
        if (tok == "i32") {
          f.m_locals[f.m_localCount++] = ParamType::_i32;
        } else if (tok == "i64") {
          f.m_locals[f.m_localCount++] = ParamType::_i64;
        } else if (tok == "f32") {
          f.m_locals[f.m_localCount++] = ParamType::_f32;
        } else if (tok == "f64") {
          f.m_locals[f.m_localCount++] = ParamType::_f64;
        } else {
          throw "Invalid local type";
        }
      }

      p = e + 1;
    }

    p = close + 1;
  }

  // ------------------------------------------------
  // BODY START
  // ------------------------------------------------
  size_t body_start = func.find(')', func.find("(func"));
  if (body_start == std::string_view::npos)
    throw "Malformed func";

  body_start++;

  // ------------------------------------------------
  // INSTRUCTION PARSING
  // ------------------------------------------------
  size_t line_start = body_start;

  for (size_t i = body_start; i < func.size(); i++) {
    if (func[i] == '\n') {

      auto line = func.substr(line_start, i - line_start);

      while (!line.empty() && line.front() == ' ')
        line.remove_prefix(1);

      while (!line.empty() && line.back() == ')')
        line.remove_suffix(1);

      if (!line.empty() && line[0] != '(') {
        if (f.m_bodyCount >= f.m_body.size())
          throw "Too many instructions";

        f.m_body[f.m_bodyCount++] = ParseInstruction(line);
        Instr &instr = f.m_body[f.m_bodyCount - 1];

        // If a block instruction
        if (instr.m_op == OP::_block || instr.m_op == OP::_loop) {
          uint32_t idx = f.m_blockIdx++;
          Block &block = f.m_blockTable[idx];
          block.m_blockStart = f.m_bodyCount - 1;
          block.m_isLoop = instr.m_op == OP::_loop;
          f.Push(idx);
        } else if (instr.m_op == OP::_end) {
          Block &block = f.Pop();
          block.m_blockEnd = f.m_bodyCount - 1;
        }
      }

      line_start = i + 1;
    }
  }

  // last line
  auto line = func.substr(line_start);

  while (!line.empty() && line.front() == ' ')
    line.remove_prefix(1);

  while (!line.empty() && line.back() == ')')
    line.remove_suffix(1);

  if (!line.empty() && line[0] != '(') {
    if (f.m_bodyCount >= f.m_body.size())
      throw "Too many instructions";

    f.m_body[f.m_bodyCount++] = ParseInstruction(line);
  }

  return f;
}

constexpr Instr ParseGlobalEntry(std::string_view entry) {
  Instr instr{};

  // ------------------------------------------------
  // ENTRY NAME
  // ------------------------------------------------
  size_t pos = entry.find("(global");
  pos += 7;

  while (pos < entry.size() && entry[pos] == ' ') {
    pos++;
  }

  size_t end = pos;
  while (end < entry.size() && entry[end] != ' ' && entry[end] != '(') {
    end++;
  }

  instr.m_operand = entry.substr(pos, end - pos);

  // ------------------------------------------------
  // MUTABILITY — skip (mut ...) or just the type
  // ------------------------------------------------
  // skip to type — find i32/i64/f32/f64
  size_t typePos = entry.find("i32", pos);
  size_t typePos64 = entry.find("i64", pos);
  size_t typeF32 = entry.find("f32", pos);
  size_t typeF64 = entry.find("f64", pos);

  size_t typeStart = std::string_view::npos;
  if (typePos != std::string_view::npos) {
    typeStart = typePos;
    instr.m_op = OP::_i32;
  }
  if (typePos64 != std::string_view::npos && typePos64 < typeStart) {
    typeStart = typePos64;
    instr.m_op = OP::_i64;
  }
  if (typeF32 != std::string_view::npos && typeF32 < typeStart) {
    typeStart = typeF32;
    instr.m_op = OP::_f32;
  }
  if (typeF64 != std::string_view::npos && typeF64 < typeStart) {
    typeStart = typeF64;
    instr.m_op = OP::_f64;
  }

  if (typeStart == std::string_view::npos)
    throw "Global type not found";

  // ------------------------------------------------
  // VALUE — find (i32.const 66576) or (f32.const ...)
  // ------------------------------------------------
  size_t constPos = entry.find(".const", typeStart);
  if (constPos == std::string_view::npos)
    throw "Global value not found";

  constPos += 6; // skip ".const"
  while (constPos < entry.size() && entry[constPos] == ' ')
    constPos++;

  // parse value
  if (instr.m_op == OP::_i32 || instr.m_op == OP::_i64) {
    int64_t value = 0;
    bool neg = false;
    if (entry[constPos] == '-') {
      neg = true;
      constPos++;
    }
    while (constPos < entry.size() && entry[constPos] >= '0' &&
           entry[constPos] <= '9') {
      value = value * 10 + (entry[constPos] - '0');
      constPos++;
    }
    instr.m_operandValue = neg ? -value : value;
  } else {
    // f32/f64 — reuse hex float parsing
    size_t valEnd = constPos;
    while (valEnd < entry.size() && entry[valEnd] != ')' &&
           entry[valEnd] != ' ')
      valEnd++;
    std::string_view floatStr = entry.substr(constPos, valEnd - constPos);
    instr.m_operandValueDecimal = ParseHexDecimal(floatStr);
  }

  return instr;
}

constexpr STATUS ParseDataEntry(std::string_view entry, Memory &memory) {

  // ------------------------------------------------
  // ADDRESS — find (i32.const 1024)
  // ------------------------------------------------
  size_t constPos = entry.find("i32.const");
  if (constPos == std::string_view::npos)
    throw "Data entry address not found";

  constPos += 9; // skip "i32.const"
  while (constPos < entry.size() && entry[constPos] == ' ')
    constPos++;

  int32_t addr = 0;
  while (constPos < entry.size() && entry[constPos] >= '0' &&
         entry[constPos] <= '9') {
    addr = addr * 10 + (entry[constPos] - '0');
    constPos++;
  }

  // ------------------------------------------------
  // BYTES — find the quoted string "\05\00\00\00"
  // ------------------------------------------------
  size_t quoteStart = entry.find('"');
  if (quoteStart == std::string_view::npos)
    throw "Data entry bytes not found";
  quoteStart++; // skip opening quote

  size_t quoteEnd = entry.find('"', quoteStart);
  if (quoteEnd == std::string_view::npos)
    throw "Data entry closing quote not found";

  std::string_view bytes = entry.substr(quoteStart, quoteEnd - quoteStart);

  // ------------------------------------------------
  // PARSE BYTES — \xx escape sequences or raw chars
  // ------------------------------------------------
  size_t i = 0;
  int32_t offset = 0;
  while (i < bytes.size()) {
    if (bytes[i] == '\\') {
      // hex escape \xx
      i++;
      auto hexDigit = [](char c) -> uint8_t {
        if (c >= '0' && c <= '9')
          return c - '0';
        if (c >= 'a' && c <= 'f')
          return c - 'a' + 10;
        if (c >= 'A' && c <= 'F')
          return c - 'A' + 10;
        return 0;
      };
      uint8_t hi = hexDigit(bytes[i++]);
      uint8_t lo = hexDigit(bytes[i++]);
      memory.m_data[addr + offset++] = static_cast<uint8_t>((hi << 4) | lo);
    } else {
      // raw character
      memory.m_data[addr + offset++] = static_cast<uint8_t>(bytes[i++]);
    }
  }

  return STATUS::OK;
}

constexpr STATUS ParseElemEntry(std::string_view entry, VirtualTable &vtable,
                                FunctionTable &functionTable) {
  // ------------------------------------------------
  // BASE INDEX — find (i32.const N)
  // ------------------------------------------------
  size_t constPos = entry.find("i32.const");
  if (constPos == std::string_view::npos)
    throw "Elem entry base index not found";

  constPos += 9; // skip "i32.const"
  while (constPos < entry.size() && entry[constPos] == ' ')
    constPos++;

  int32_t baseIdx = 0;
  while (constPos < entry.size() && entry[constPos] >= '0' &&
         entry[constPos] <= '9') {
    baseIdx = baseIdx * 10 + (entry[constPos] - '0');
    constPos++;
  }

  // ------------------------------------------------
  // FUNCTION NAMES — collect all $func names after "func"
  // ------------------------------------------------
  size_t funcKeyword = entry.find("func", constPos);
  if (funcKeyword == std::string_view::npos) {
    throw "Elem entry func keyword not found";
  }

  funcKeyword += 4; // skip "func"

  int32_t offset = 0;
  size_t pos = funcKeyword;

  while (pos < entry.size()) {
    // skip whitespace
    while (pos < entry.size() && entry[pos] == ' ') {
      pos++;
    }

    // end of entry
    if (pos >= entry.size() || entry[pos] == ')') {
      break;
    }

    // must start with $
    if (entry[pos] != '$') {
      break;
    }

    // extract function name
    size_t nameStart = pos;
    while (pos < entry.size() && entry[pos] != ' ' && entry[pos] != ')') {
      pos++;
    }

    std::string_view funcName = entry.substr(nameStart, pos - nameStart);

    // look up function in function table by hash
    size_t hash = constexpr_hash(funcName);
    int32_t idx = static_cast<int32_t>(hash % MAXFUNCTIONS);
    Function &f = functionTable.m_data[idx];

    // store in virtual table
    int32_t tableIdx = baseIdx + offset;
    if (tableIdx >= MAXVIRTUALTABLESIZE) {
      throw "Virtual table overflow";
    }

    vtable.m_data[tableIdx] = &f;
    offset++;
  }

  return STATUS::OK;
}
// ----- MODULE BUILDER -----
inline constexpr uint32_t SetupModule(std::string_view child, State &state) {
  WASMOP type = Identify(child);
  if (type == WASMOP::_func) {
    // Extract and set function
    Function f = ParseFunction(child);
    std::size_t hash = constexpr_hash(f.m_name);
    int64_t h = static_cast<int64_t>(hash & 0x7fffffffffffffffULL);
    state.m_functionTable.m_data[h % MAXFUNCTIONS] = f;
    state.m_functionTable.m_count++;
  } else if (type == WASMOP::_type) {
  } else if (type == WASMOP::_table) {
  } else if (type == WASMOP::_memory) {
  } else if (type == WASMOP::_global) {
    // Extract and set global
    Instr instr = ParseGlobalEntry(child);
    std::size_t hash = constexpr_hash(instr.m_operand);
    int64_t h = static_cast<int64_t>(hash % GLOBALSIZE);

    Data globalEntry;
    if (instr.m_op == OP::_i32) {
      globalEntry.m_data = static_cast<int32_t>(instr.m_operandValue);
    } else if (instr.m_op == OP::_i64) {
      globalEntry.m_data = static_cast<int64_t>(instr.m_operandValue);
    } else if (instr.m_op == OP::_f32) {
      globalEntry.m_data = static_cast<float>(instr.m_operandValueDecimal);
    } else {
      globalEntry.m_data = static_cast<double>(instr.m_operandValueDecimal);
    }
    state.m_global.m_data[h % MAXFUNCTIONS] = globalEntry;
    state.m_global.m_count++;

    if (instr.m_operand == "$__stack_pointer") {
      // Function Stack Initialization
      Stack &stk = state.m_stack;
      stk.m_basePointer = instr.m_operandValue;
      stk.m_stackPointer = instr.m_operandValue;
      stk.m_floorPointer = instr.m_operandValue;

      // Operand Stack Initialization
      Stack &op_stk = state.m_opStack;
      op_stk.m_basePointer = instr.m_operandValue;
      op_stk.m_stackPointer = instr.m_operandValue;
      op_stk.m_floorPointer = instr.m_operandValue;
    }
  } else if (type == WASMOP::_export) {
  } else if (type == WASMOP::_import) {
  } else if (type == WASMOP::_data) {
    STATUS res = ParseDataEntry(child, state.m_memory);
    if (res != STATUS::OK) {
      throw "data segemnet parse falied\n";
    }
  } else if (type == WASMOP::_elem) {
    STATUS res =
        ParseElemEntry(child, state.m_virtualTable, state.m_functionTable);
    if (res != STATUS::OK) {
      throw "data segemnet parse falied\n";
    }
  } else {
    throw "Unknown module child!";
  }
  return 0;
}