#pragma once
#include "constants.hpp"
#include "state.hpp"
#include "types.hpp"
#include <array>
#include <cstdint>
#include <string_view>

inline consteval std::pair<uint32_t, std::array<Span, MAXMODULES>>
ParseProgram(std::string_view module) {
  std::array<Span, MAXMODULES> modules{};
  uint32_t start = 0, count = 0, open = 0;
  for (uint32_t i = 0; i < module.size(); i++) {
    char c = module[i];
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

// ----- MODULE PARSER -----
inline consteval uint32_t
ParseModuleItems(std::string_view module,
                 std::array<Span, MAXCHILDREN> &items) {
  uint32_t open = 0, start = 0, count = 0;
  for (uint32_t i = 0; i < module.size(); ++i) {
    char c = module[i];
    if (c == '(') {
      if (open == 0) { // start of a module child
        start = i;
      }
      open++;
    }
    if (c == ')') {
      if (open == 0)
        throw "Unbalanced parentheses";
      open--;
      if (count < MAXCHILDREN) {
        if (open == 0) { // closing module child
          items[count] = {start, i + 1};
          count++;
        }
      }
    }
  }
  return count;
}

// ----- MODULE VALIDATOR -----
inline consteval uint32_t ParseModule(std::string_view module,
                                      std::array<Span, MAXCHILDREN> &items) {
  uint32_t count = ParseModuleItems(module, items);
  for (uint32_t i = 0; i < count; ++i) {
    Span span = items[i];
    std::string_view child = module.substr(span.begin, span.end - span.begin);
    WASMOP type = Identify(child);

    if (type == WASMOP::_unknown) {
      // throw if invalid child
      throw "Unknown module child!";
    }
  }

  return count;
}

consteval std::pair<std::string_view, std::string_view>
split2(std::string_view s) {
  size_t pos = s.find(' ');
  if (pos == std::string_view::npos)
    return {s, {}};

  return {s.substr(0, pos), s.substr(pos + 1)};
}

consteval Instr ParseInstruction(std::string_view line) {
  auto [op, rest] = split2(line);
  Instr instr{op, rest};
  return instr;
}

consteval Function ParseFunction(std::string_view func) {
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

constexpr std::size_t constexpr_hash(std::string_view str) {
  std::size_t hash = 14695981039346656037ull; // FNV offset basis
  for (char c : str) {
    hash ^= static_cast<std::size_t>(c);
    hash *= 1099511628211ull; // FNV prime
  }
  return hash;
}

// ----- MODULE BUILDER -----
inline consteval uint32_t SetupModule(std::string_view child, State &state) {
  WASMOP type = Identify(child);
  if (type == WASMOP::_func) {
    // Extract and set function
    Function f = ParseFunction(child);
    if (f.m_typeIndex >= MAXFUNCTIONS) {
      throw "Function index out of table range";
    }
    std::size_t hash = constexpr_hash(f.m_name);
    state.m_functionTable.m_data[hash % MAXFUNCTIONS] = f;
  } else if (type == WASMOP::_type) {
  } else if (type == WASMOP::_table) {
  } else if (type == WASMOP::_memory) {
  } else if (type == WASMOP::_global) {
  } else if (type == WASMOP::_export) {
  } else {
    throw "Unknown module child!";
  }
  return 0;
}

constexpr STATUS HandleCall(State &state, const std::string_view &funcName) {
  Stack &stk = state.m_stack;
  Instr iPtr = Instr{OP::_i64, Member::_none, OperandType::_immediate, "0",
                     state.m_instrPointer};
  // Push return address to recover it after function call
  stk.Push(iPtr);
  Instr bPtr = Instr{OP::_i64, Member::_none, OperandType::_immediate, "0",
                     stk.m_basePointer};
  // Push the base ptr to recover it after function call
  stk.Push(bPtr);

  // Update the base pointer to the current stack pointer for the callee
  stk.m_basePointer = stk.m_stackPointer;

  // Push the function arguments
  // get hash of the function name and use it to get the function type and
  // argument count
  size_t hash = constexpr_hash(funcName);
  Function &f = state.m_functionTable.m_data[hash % MAXFUNCTIONS];
  uint32_t argCount = f.m_paramCount;
  uint32_t localCount = f.m_localCount;

  // TODO: Get the actual argument value from the function arguments instead of
  // pushing 0
  for (int i = 0; i < argCount; i++) {
    ParamType &param = f.m_params[i];
    if (param == ParamType::_i32) {
      stk.Push(Instr{OP::_i32, Member::_none, OperandType::_immediate, "0"});
    } else if (param == ParamType::_i64) {
      stk.Push(Instr{OP::_i64, Member::_none, OperandType::_immediate, "0"});
    } else if (param == ParamType::_f32) {
      stk.Push(Instr{OP::_f32, Member::_none, OperandType::_immediate, "0"});
    } else if (param == ParamType::_f64) {
      stk.Push(Instr{OP::_f64, Member::_none, OperandType::_immediate, "0"});
    } else if (param == ParamType::_funcref) {
      stk.Push(Instr{OP::_i32, Member::_none, OperandType::_address, "0"});
    } else if (param == ParamType::_externref) {
      stk.Push(Instr{OP::_i64, Member::_none, OperandType::_address, "0"});
    } else {
      throw "Unsupported parameter type";
    }
  }
  // Push local variables
  for (int i = 0; i < localCount; i++) {
    ParamType &local = f.m_locals[i];
    if (local == ParamType::_i32) {
      stk.Push(Instr{OP::_i32, Member::_none, OperandType::_immediate, "0"});
    } else if (local == ParamType::_i64) {
      stk.Push(Instr{OP::_i64, Member::_none, OperandType::_immediate, "0"});
    } else if (local == ParamType::_f32) {
      stk.Push(Instr{OP::_f32, Member::_none, OperandType::_immediate, "0"});
    } else if (local == ParamType::_f64) {
      stk.Push(Instr{OP::_f64, Member::_none, OperandType::_immediate, "0"});
    } else if (local == ParamType::_funcref) {
      stk.Push(Instr{OP::_i32, Member::_none, OperandType::_address, "0"});
    } else if (local == ParamType::_externref) {
      stk.Push(Instr{OP::_i64, Member::_none, OperandType::_address, "0"});
    } else {
      throw "Unsupported local variable type";
    }
  }

  // Update the active Function and instrunction Pointer
  state.m_activeFunction = &f;
  state.m_instrPointer = 0;
  return STATUS::OK;
}

consteval STATUS HandleLocal(State &state, const Instr &instr) {
  Stack &stk = state.m_stack;
  uint64_t address = stk.m_basePointer - instr.m_operandValue - 1;
  if (address < 0) {
    throw "Invalid Address used in local.get";
  }
  if (instr.m_mem == Member::_get) {
    Instr instr = Instr{OP::_i64, Member::_none, OperandType::_immediate, "0",
                        stk.m_data[address].m_operandValue};
    stk.Push(instr);
  } else {
    Instr instr = stk.Pop();
    stk.m_data[address] = instr;
  }
  return STATUS::OK;
}

consteval STATUS HandleGlobal(State &state, const Instr &instr) {
  
  return STATUS::OK;
}

consteval STATUS run(State &state) {

  // Get the main!
  size_t hash = constexpr_hash("$main");
  Function &f = state.m_functionTable.m_data[hash % MAXFUNCTIONS];
  uint32_t instrCount = f.m_bodyCount;
  state.m_activeFunction = &f;

  while(1) {
    Instr _op = state.m_activeFunction->m_body[state.m_instrPointer];
    switch (_op.m_op) {
    case OP::_nop: {
      continue;
    }
    case OP::_drop: {
    }
    case OP::_local: {
      STATUS res = HandleLocal(state, _op);
      if (res == STATUS::ERROR) {
        throw "LOCAL Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_global: {
      STATUS res = HandleGlobal(state, _op);
      if (res == STATUS::ERROR) {
        throw "LOCAL Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_select: {
      // using type = SELECT<True, float, int>::type;
      // return std::is_same_v<type, int>;
      break;
    }
    case OP::_call: {
      STATUS res = HandleCall(state, _op.m_operand);
      if (res == STATUS::ERROR) {
        throw "CALL Call Handling Failed!";
        return STATUS::ERROR;
      }
      break;
    }
    case OP::_unreachable: {
      throw "CPU Halt!";
      return STATUS::ERROR;
    }
    }
  }
  return STATUS::OK;
}

inline consteval STATUS Run() {
  State state{};
  auto res = ParseProgram(program);
  int m_count = res.first;

  if (m_count == 0)
    throw "No module found";

  if (m_count > MAXMODULES)
    throw "Too many modules found";

  // max MAXCHILDREN module children and MAXMODULES modules
  // Validate Modules
  std::array<std::array<Span, MAXCHILDREN>, MAXMODULES> items{};
  std::array<int, MAXMODULES> itemCount{};
  for (int moduleIdx = 0; moduleIdx < m_count; moduleIdx++) {
    Span span = res.second[moduleIdx];
    std::string_view module = program.substr(span.begin, span.end - span.begin);
    uint32_t c_count = ParseModule(module, items[moduleIdx]);

    if (c_count > MAXCHILDREN) {
      throw "Too many children found";
    }
    itemCount[moduleIdx] = c_count;
  }

  // Setup Modules
  for (int moduleIdx = 0; moduleIdx < m_count; moduleIdx++) {
    auto span = res.second[moduleIdx];
    std::string_view module = program.substr(span.begin, span.end - span.begin);
    for (int item = 0; item < itemCount[moduleIdx]; item++) {
      auto moduleItem = items[moduleIdx][item];
      std::string_view child =
          module.substr(moduleItem.begin, moduleItem.end - moduleItem.begin);
      int c_res = SetupModule(child, state);

      if (c_res != 0) {
        throw "Invalid child format";
      }
    }
  }

  // End of memory (64KB) - STACKSIZE (8KB)
  // TODO: Remove hard coded initalisaiton
  Stack &stk = state.m_stack;
  stk.m_stackPointer = 66560 - 1;
  stk.m_basePointer = 66560 - 1;

  STATUS setupCall = HandleCall(state, "$main");
  if (setupCall != STATUS::OK) {
    throw "setup call failed\n";
  }

  state.m_instrPointer = 0;
  return run(state);
}