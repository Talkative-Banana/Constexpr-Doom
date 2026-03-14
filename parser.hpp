#pragma once
#include "constants.hpp"
#include "handler.hpp"
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
split2(std::string_view s, char delimiter = ' ') {
  size_t pos = s.find(delimiter);
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