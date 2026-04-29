"""
WAT Runtime Parser — emits a C++ make_state() function that
directly initialises every field of your State struct.

Usage:
    python runtimeParser.py test.wat             # prints to stdout
    python runtimeParser.py test.wat out.hpp     # writes header
"""

from __future__ import annotations
import sys, re, math
from dataclasses import dataclass, field
from enum import Enum, auto
from typing import Optional, Union

# ──────────────────────────────────────────────────────────────
#  Constants — keep in sync with constants.hpp
# ──────────────────────────────────────────────────────────────
MAXFUNCTIONS        = 800
GLOBALSIZE          = 650
MEMORYSIZE          = 1024 * 1024 * 8
MAXVIRTUALTABLESIZE = 700
STACKSIZE           = 1024 * 528
BLOCKSIZE           = 512
BLOCKSTACKSIZE      = 512
MAXNUMPARAMS        = 32
BRTABLESIZE         = 512
FDTABLE             = 16
FILESYSTEMSIZE     = 1024 * 600   # 600 KB

# ──────────────────────────────────────────────────────────────
#  Enums
# ──────────────────────────────────────────────────────────────
class WASMOP(Enum):
    _unknown=auto();_func=auto();_table=auto();_type=auto()
    _memory=auto();_global=auto();_import=auto();_export=auto()
    _data=auto();_elem=auto()

class OP(Enum):
    _nop=auto();_drop=auto();_select=auto();_unreachable=auto()
    _block=auto();_end=auto();_loop=auto();_if=auto()
    _br=auto();_br_if=auto();_br_table=auto()
    _call=auto();_call_indirect=auto();_return=auto()
    _local=auto();_global=auto()
    _i32=auto();_i64=auto();_f32=auto();_f64=auto()
    _memory=auto();_data=auto()

class Member(Enum):
    _none=auto();_get=auto();_set=auto();_tee=auto();_const=auto()
    _add=auto();_sub=auto();_mul=auto()
    _div_s=auto();_div_u=auto();_div=auto()
    _rem_s=auto();_rem_u=auto()
    _and=auto();_or=auto();_xor=auto()
    _shl=auto();_shr_s=auto();_shr_u=auto()
    _eqz=auto();_eq=auto();_ne=auto()
    _lt_s=auto();_lt_u=auto();_lt=auto()
    _gt_s=auto();_gt_u=auto();_gt=auto()
    _le_s=auto();_le_u=auto();_le=auto()
    _ge_s=auto();_ge_u=auto();_ge=auto()
    _store=auto();_load=auto()
    _load8_u=auto();_load8_s=auto();_load16_u=auto();_load16_s=auto()
    _store8=auto();_store16=auto()
    _trunc_f32_s=auto();_trunc_f64_s=auto()
    _convert_i32_s=auto();_convert_i64_s=auto()
    _promote_f32=auto();_demote_f64=auto()
    _extend_i32_s=auto();_wrap_i64=auto()
    _abs=auto();_neg=auto();_sqrt=auto();_min=auto();_max=auto()
    _clz=auto();_ctz=auto();_popcnt=auto()

class ParamType(Enum):
    _i32=auto();_i64=auto();_f32=auto();_f64=auto()
    _funcref=auto();_externref=auto()

class OperandType(Enum):
    _none=auto();_immediate=auto();_address=auto();_offset=auto();_hash=auto()

# ──────────────────────────────────────────────────────────────
#  Python data models
# ──────────────────────────────────────────────────────────────
@dataclass
class Block:
    block_start: int  = 0
    block_end:   int  = 0
    is_loop:     bool = False

@dataclass
class Instr:
    op:                OP          = OP._nop
    member:            Member      = Member._none
    operand_type:      OperandType = OperandType._none
    operand:           str         = ""
    operand_value:     int         = 0
    operand_value_dec: float       = 0.0
    br_table:          list[int]   = field(default_factory=list)
    br_count:          int         = 0

@dataclass
class FuncType:
    params:  list[ParamType] = field(default_factory=list)
    results: list[ParamType] = field(default_factory=list)

@dataclass
class Function:
    name:          str             = ""
    type_index:    int             = 0
    params:        list[ParamType] = field(default_factory=list)
    locals:        list[ParamType] = field(default_factory=list)
    body:          list[Instr]     = field(default_factory=list)
    block_table:   list[Block]     = field(default_factory=list)
    block_idx:     int             = 0   # == number of blocks seen (m_blockIdx)
    is_defined:    bool            = False

@dataclass
class GlobalEntry:
    name:  str
    type:  ParamType
    value: Union[int, float]

@dataclass
class Memory:
    data: bytearray = field(default_factory=lambda: bytearray(MEMORYSIZE))

@dataclass
class FileDesc:
    m_open: bool = False
    m_offset: int = 0
    m_size: int = 0
    m_dataPtr: int = 0

@dataclass
class Descriptor:
    m_fdTable: list[FileDesc] = field(default_factory=lambda: [FileDesc() for _ in range(FDTABLE)])

@dataclass
class FileSystem:
    m_data: bytearray = field(default_factory=lambda: bytearray(FILESYSTEMSIZE))

@dataclass
class ParsedState:
    functions:      dict[str, Function]     = field(default_factory=dict)
    globals:        dict[str, GlobalEntry]  = field(default_factory=dict)
    memory:         Memory                  = field(default_factory=Memory)
    fileSystem:     FileSystem              = field(default_factory=FileSystem)
    vtable_entries: list[tuple[int, str]]   = field(default_factory=list)
    stack_pointer:  int                     = 0

# ──────────────────────────────────────────────────────────────
#  Lookup tables
# ──────────────────────────────────────────────────────────────
_OP_MAP: dict[str, OP] = {
    "local":OP._local,"global":OP._global,
    "i32":OP._i32,"i64":OP._i64,"f32":OP._f32,"f64":OP._f64,
    "return":OP._return,"call":OP._call,"call_indirect":OP._call_indirect,
    "block":OP._block,"loop":OP._loop,"end":OP._end,
    "br_if":OP._br_if,"br":OP._br,"br_table":OP._br_table,
    "drop":OP._drop,"select":OP._select,
    "unreachable":OP._unreachable,"halt":OP._unreachable,
    "nop":OP._nop,"if":OP._if,
}
_MEM_MAP: dict[str, Member] = {
    "get":Member._get,"set":Member._set,"tee":Member._tee,"const":Member._const,
    "add":Member._add,"sub":Member._sub,"mul":Member._mul,
    "div_s":Member._div_s,"div_u":Member._div_u,"div":Member._div,
    "rem_s":Member._rem_s,"rem_u":Member._rem_u,
    "and":Member._and,"or":Member._or,"xor":Member._xor,
    "shl":Member._shl,"shr_s":Member._shr_s,"shr_u":Member._shr_u,
    "eqz":Member._eqz,"eq":Member._eq,"ne":Member._ne,
    "lt_s":Member._lt_s,"lt_u":Member._lt_u,"lt":Member._lt,
    "gt_s":Member._gt_s,"gt_u":Member._gt_u,"gt":Member._gt,
    "le_s":Member._le_s,"le_u":Member._le_u,"le":Member._le,
    "ge_s":Member._ge_s,"ge_u":Member._ge_u,"ge":Member._ge,
    "store":Member._store,"load":Member._load,
    "load8_u":Member._load8_u,"load8_s":Member._load8_s,
    "load16_u":Member._load16_u,"load16_s":Member._load16_s,
    "store8":Member._store8,"store16":Member._store16,
    "trunc_f32_s":Member._trunc_f32_s,"trunc_f64_s":Member._trunc_f64_s,
    "convert_i32_s":Member._convert_i32_s,"convert_i64_s":Member._convert_i64_s,
    "promote_f32":Member._promote_f32,"demote_f64":Member._demote_f64,
    "extend_i32_s":Member._extend_i32_s,"wrap_i64":Member._wrap_i64,
    "abs":Member._abs,"neg":Member._neg,"sqrt":Member._sqrt,
    "min":Member._min,"max":Member._max,
    "clz":Member._clz,"ctz":Member._ctz,"popcnt":Member._popcnt,
}
_TYPE_MAP: dict[str, ParamType] = {
    "i32":ParamType._i32,"i64":ParamType._i64,
    "f32":ParamType._f32,"f64":ParamType._f64,
    "funcref":ParamType._funcref,"externref":ParamType._externref,
}

# ──────────────────────────────────────────────────────────────
#  Helpers
# ──────────────────────────────────────────────────────────────
def strip_comments(src: str) -> str:
    while True:
        s = src.find("(;");
        if s == -1: break
        e = src.find(";)", s)
        if e == -1: break
        src = src[:s] + " " * (e+2-s) + src[e+2:]
    out = []
    for line in src.splitlines():
        idx = line.find(";;")
        out.append(line[:idx] if idx != -1 else line)
    return "\n".join(out)

def split_module_items(src: str):
    items = []
    depth = 0
    start = None

    in_string = False
    escape = False

    for i, c in enumerate(src):

        # --- handle string ---
        if in_string:
            if escape:
                escape = False
            elif c == '\\':
                escape = True
            elif c == '"':
                in_string = False
            continue

        if c == '"':
            in_string = True
            continue

        # --- handle parentheses ---
        if c == '(':
            if depth == 0:
                start = i
            depth += 1

        elif c == ')':
            depth -= 1
            if depth == 0 and start is not None:
                items.append(src[start:i+1])
                start = None

    return items

def extract_module(src: str) -> str:
    start = src.find("(module")
    if start == -1:
        raise ValueError("No (module) found")

    depth = 0
    in_string = False
    escape = False

    for i in range(start, len(src)):
        c = src[i]

        # --- string handling ---
        if in_string:
            if escape:
                escape = False
            elif c == '\\':
                escape = True
            elif c == '"':
                in_string = False
            continue

        if c == '"':
            in_string = True
            continue

        # --- parentheses ---
        if c == '(':
            depth += 1
        elif c == ')':
            depth -= 1
            if depth == 0:
                return src[start:i+1]

    raise ValueError("Unclosed (module)")

def identify(block: str) -> WASMOP:
    s = block.lstrip()
    if s.startswith("(func"):   return WASMOP._func
    if s.startswith("(type"):   return WASMOP._type
    if s.startswith("(table"):  return WASMOP._table
    if s.startswith("(memory"): return WASMOP._memory
    if s.startswith("(global"): return WASMOP._global
    if s.startswith("(export"): return WASMOP._export
    if s.startswith("(data"):   return WASMOP._data
    if s.startswith("(elem"):   return WASMOP._elem
    if s.startswith("(import"): return WASMOP._import
    return WASMOP._unknown

def parse_hex_decimal(s: str) -> float:
    s = re.sub(r'\(;.*?;\)', '', s).strip()
    neg = s.startswith('-')
    if neg: s = s[1:]
    try: result = float.fromhex(s)
    except ValueError: result = float(s)
    return -result if neg else result

# ──────────────────────────────────────────────────────────────
#  Instruction parser
# ──────────────────────────────────────────────────────────────
def parse_instruction(line: str) -> Instr:
    line = line.strip()
    while line.endswith(")"):
        line = line[:-1].rstrip()
    parts  = line.split(None, 1)
    op_str = parts[0] if parts else ""
    rest   = parts[1].strip() if len(parts) > 1 else ""
    instr  = Instr(operand=rest)
    dot    = op_str.find(".")
    _op    = op_str[:dot] if dot != -1 else op_str
    _mem   = op_str[dot+1:] if dot != -1 else ""
    instr.op     = _OP_MAP.get(_op, OP._nop)
    instr.member = _MEM_MAP.get(_mem, Member._none) if _mem else Member._none

    if instr.op in (OP._block,OP._loop,OP._select,OP._end,
                    OP._drop,OP._return,OP._unreachable,OP._nop,OP._if):
        return instr

    if instr.op == OP._br_table:
        nums = re.findall(r'\b(\d+)\b', rest)
        instr.br_table = [int(n) for n in nums]
        instr.br_count = len(instr.br_table)
        return instr

    is_int = instr.op in (OP._i32, OP._i64)
    is_dec = instr.op in (OP._f32, OP._f64)

    if is_int and instr.member == Member._const:
        m = re.search(r'-?\d+', rest)
        instr.operand_value = int(m.group()) if m else 0
        instr.operand_type  = OperandType._immediate
        return instr

    if is_dec and instr.member == Member._const:
        instr.operand_value_dec = parse_hex_decimal(rest) if rest else 0.0
        instr.operand_type = OperandType._immediate
        return instr

    if instr.op in (OP._local, OP._global):
        if rest and rest[0].isdigit():
            instr.operand_value = int(re.match(r'\d+', rest).group())
            instr.operand_type  = OperandType._immediate
        elif rest.startswith("$"):
            instr.operand_type = OperandType._address
        return instr

    if instr.op in (OP._call, OP._br, OP._br_if, OP._call_indirect):
        if rest and rest[0].isdigit():
            instr.operand_value = int(re.match(r'\d+', rest).group())
            instr.operand_type  = OperandType._immediate
        elif rest.startswith("$"):
            instr.operand_type = OperandType._address
        return instr

    if rest.startswith("offset="):
        m = re.match(r'offset=(\d+)', rest)
        instr.operand_value = int(m.group(1)) if m else 0
        instr.operand_type  = OperandType._offset
    elif rest and rest[0].isdigit():
        instr.operand_value = int(re.match(r'\d+', rest).group())
        instr.operand_type  = OperandType._immediate
    elif rest.startswith("$"):
        instr.operand_type = OperandType._address

    return instr

# ──────────────────────────────────────────────────────────────
#  Section parsers
# ──────────────────────────────────────────────────────────────
def parse_type(src: str) -> FuncType:
    ft = FuncType()
    # find the inner (func ...) — may contain nested (param) and (result)
    m = re.search(r'\(func(.*)\)', src, re.DOTALL)
    if not m:
        return ft
    inner = m.group(1)
    for pm in re.finditer(r'\(param([^)]*)\)', inner):
        for t in pm.group(1).split():
            if t in _TYPE_MAP: ft.params.append(_TYPE_MAP[t])
    for rm in re.finditer(r'\(result([^)]*)\)', inner):
        for t in rm.group(1).split():
            if t in _TYPE_MAP: ft.results.append(_TYPE_MAP[t])
    return ft

def parse_function(src: str) -> Function:
    fn = Function(is_defined=True)
    m = re.match(r'\(func\s+(\$[^\s()]+)', src)
    fn.name = m.group(1) if m else ""
    m = re.search(r'\(type\s+(\d+)\)', src)
    fn.type_index = int(m.group(1)) if m else 0
    for pm in re.finditer(r'\(param([^)]*)\)', src):
        for t in pm.group(1).split():
            if t in _TYPE_MAP: fn.params.append(_TYPE_MAP[t])
    for lm in re.finditer(r'\(local([^)]*)\)', src):
        for t in lm.group(1).split():
            if t in _TYPE_MAP: fn.locals.append(_TYPE_MAP[t])

    body_lines: list[str] = []
    in_header = True
    for line in src.splitlines():
        s = line.strip()
        if in_header:
            if s.startswith("(func"): in_header = False
            continue
        if not s: continue
        if s.startswith(("(param","(local","(type","(result")): continue
        body_lines.append(s)

    block_stack: list[int] = []
    for raw in body_lines:
        line = raw.strip()
        if not line or line.startswith("(;"): continue
        while line.endswith(")") and not line.startswith("("):
            line = line[:-1].rstrip()
        if not line: continue

        instr = parse_instruction(line)
        idx = len(fn.body)
        fn.body.append(instr)

        if instr.op in (OP._block, OP._loop):
            fn.block_table.append(Block(block_start=idx, is_loop=(instr.op==OP._loop)))
            block_stack.append(len(fn.block_table)-1)
            fn.block_idx += 1
        elif instr.op == OP._end and block_stack:
            fn.block_table[block_stack.pop()].block_end = idx

    return fn

def parse_import(src: str, type_table: dict[int, FuncType] = {}) -> Function:
    fn = Function(is_defined=False)

    # (import "env" "strlen" (func $strlen (type 3)))
    strings = re.findall(r'"([^"]*)"', src)
    if len(strings) >= 2:
        fn.import_module = strings[0]   # "env"
        fn.import_field  = strings[1]   # "strlen"

    m = re.search(r'\(func\s+(\$[^\s()]+)', src)
    fn.name = m.group(1) if m else ("$" + fn.import_field if fn.import_field else "$anon")

    m = re.search(r'\(type\s+(\d+)\)', src)
    fn.type_index = int(m.group(1)) if m else 0

    # fill params from type table
    if fn.type_index in type_table:
        fn.params = list(type_table[fn.type_index].params)

    return fn

def parse_global(src: str) -> GlobalEntry:
    m = re.match(r'\(global\s+(\$[\w.]+)', src)
    name = m.group(1) if m else "$anon"
    gtype = ParamType._i32
    for tname, tval in _TYPE_MAP.items():
        if tname in src: gtype = tval; break
    cm = re.search(r'\.\s*const\s+(-?[\w.+\-x]+)', src)
    if cm:
        raw = cm.group(1)
        value: Union[int,float] = (
            parse_hex_decimal(raw) if gtype in (ParamType._f32,ParamType._f64)
            else (int(raw) if re.match(r'-?\d+$', raw) else 0))
    else:
        value = 0
    return GlobalEntry(name=name, type=gtype, value=value)

def parse_data(src: str, memory: Memory):
    m = re.search(r'i32\.const\s+(\d+)', src)
    if not m: return
    addr = int(m.group(1))
    qs, qe = src.find('"'), src.rfind('"')
    if qs == qe: return
    raw, out, i = src[qs+1:qe], bytearray(), 0
    while i < len(raw):
        if raw[i] == '\\':
            out.append((int(raw[i+1],16)<<4)|int(raw[i+2],16)); i+=3
        else:
            out.append(ord(raw[i])); i+=1
    for off, byte in enumerate(out):
        if addr+off < len(memory.data): memory.data[addr+off] = byte

def parse_elem(src: str) -> list[tuple[int,str]]:
    m = re.search(r'i32\.const\s+(\d+)', src)
    base = int(m.group(1)) if m else 0
    fk = src.find("func")
    if fk == -1: return []
    names = re.findall(r'(\$[\w.]+)', src[fk:])
    return [(base+i, n) for i, n in enumerate(names)]

# ──────────────────────────────────────────────────────────────
#  Build
# ──────────────────────────────────────────────────────────────
def build_state(src: str) -> ParsedState:
    src = strip_comments(src)
    state = ParsedState()

    module_src = extract_module(src)
    inner = module_src[len("(module"): -1]
    children = split_module_items(inner)

    # ── Pass 1: collect type table ─────────────────────────────
    type_table: dict[int, FuncType] = {}
    type_idx = 0
    for child in children:
        if identify(child) == WASMOP._type:
            type_table[type_idx] = parse_type(child)
            type_idx += 1

    # ── Pass 2: everything else ────────────────────────────────
    for child in children:
        wtype = identify(child)
        if   wtype == WASMOP._func:
            fn = parse_function(child);          state.functions[fn.name] = fn
        elif wtype == WASMOP._import:
            fn = parse_import(child, type_table); state.functions[fn.name] = fn
        elif wtype == WASMOP._global:
            g = parse_global(child);             state.globals[g.name] = g
            if g.name == "$__stack_pointer":     state.stack_pointer = int(g.value)
        elif wtype == WASMOP._data:
            parse_data(child, state.memory)
        elif wtype == WASMOP._elem:
            state.vtable_entries += parse_elem(child)

    return state

# ──────────────────────────────────────────────────────────────
#  C++ emitter helpers
# ──────────────────────────────────────────────────────────────
def q(s: str) -> str:
    """C++ string_view literal."""
    return '"' + s.replace('\\','\\\\').replace('"','\\"') + '"'

def cpp_float(v: float) -> str:
    if math.isnan(v):  return "std::numeric_limits<double>::quiet_NaN()"
    if math.isinf(v):  return ("std::numeric_limits<double>::infinity()"
                               if v > 0 else "-std::numeric_limits<double>::infinity()")
    return repr(v)

def pt(p: ParamType) -> str: return f"ParamType::{p.name}"
def ot(o: OperandType) -> str: return f"OperandType::{o.name}"

# ──────────────────────────────────────────────────────────────
#  Main emitter
# ──────────────────────────────────────────────────────────────
def emit_state(state: ParsedState, var: str = "state") -> str:
    W = []
    def w(*args): W.append("".join(str(a) for a in args))

    # ── Load WAD bytes first so we know the size ───────────────
    try:
        with open("doom1.wad", "rb") as f:
            wad_bytes = f.read()
        print(f"Loaded doom1.wad: {len(wad_bytes)} bytes", file=sys.stderr)
    except FileNotFoundError:
        wad_bytes = b""
        print("Warning: doom1.wad not found, skipping file system init", file=sys.stderr)

    # ── Header ────────────────────────────────────────────────
    w("// Auto-generated by wat_parser.py — do not edit")
    w("#pragma once")
    w('#include "state.hpp"')
    w("#include <limits>")
    w("")

    # ── WAD array at namespace scope (MUST be outside any function) ──
    if wad_bytes:
        w(f"// ════ WAD data ({len(wad_bytes)} bytes) — namespace scope so GCC can parse it ════")
        w(f"static constexpr uint8_t wad_data[{len(wad_bytes)}] = {{")
        for addr in range(0, len(wad_bytes), 16):
            group = wad_bytes[addr:addr+16]
            row = "  " + ", ".join(f"0x{b:02X}" for b in group)
            # add trailing comma on every row except the last
            if addr + 16 < len(wad_bytes):
                row += ","
            w(row)
        w("};")
        w("")

    w("inline constexpr int make_state(State& state) {")
    w("")

    # ── Functions ──────────────────────────────────────────────
    w(f"  // ════ Function Table ({len(state.functions)} functions) ════")
    fnslot = 0
    fn_slot = {}
    for fname, fn in state.functions.items():
        slot = fnslot
        ref  = f"{var}.m_functionTable.m_data[{slot}]"
        w(f"  // [{slot}] {fname!r}  (defined={fn.is_defined})")
        w(f"  {{")
        w(f"    auto& F = {ref};")
        w(f"    F.m_name          = {q(fn.name)};")
        w(f"    F.m_typeIndex     = {fn.type_index};")
        w(f"    F.m_isDefined     = {'true' if fn.is_defined else 'false'};")
        w(f"    F.m_paramCount    = {len(fn.params)};")
        for i, p in enumerate(fn.params):
            w(f"    F.m_params[{i}]     = {pt(p)};")
        w(f"    F.m_localCount    = {len(fn.locals)};")
        for i, l in enumerate(fn.locals):
            w(f"    F.m_locals[{i}]     = {pt(l)};")
        w(f"    F.m_bodyCount     = {len(fn.body)};")
        w(f"    F.m_blockIdx      = {fn.block_idx};")
        # instructions
        for ii, instr in enumerate(fn.body):
            r = f"F.m_body[{ii}]"
            w(f"    {r}.m_op                 = OP::{instr.op.name};")
            w(f"    {r}.m_mem                = Member::{instr.member.name};")
            w(f"    {r}.m_type               = {ot(instr.operand_type)};")
            if instr.operand:
                w(f"    {r}.m_operand             = {q(instr.operand)};")
            if instr.operand_value != 0:
                w(f"    {r}.m_operandValue        = {instr.operand_value}LL;")
            if instr.operand_value_dec != 0.0:
                w(f"    {r}.m_operandValueDecimal = {cpp_float(instr.operand_value_dec)};")
            if instr.br_count:
                w(f"    {r}.m_brCount             = {instr.br_count};")
                w(f"    {r}.m_brTableOffset       = {var}.m_brTablePool.m_ptr;")
                for bv in instr.br_table:
                    w(f"    {var}.m_brTablePool.m_data[{var}.m_brTablePool.m_ptr++] = {bv};")
        # block table
        for bi, blk in enumerate(fn.block_table):
            r = f"F.m_blockTable[{bi}]"
            w(f"    {r}.m_blockStart = {blk.block_start};")
            w(f"    {r}.m_blockEnd   = {blk.block_end};")
            w(f"    {r}.m_isLoop     = {'true' if blk.is_loop else 'false'};")
        w(f"    F.m_blockStackPointer = {BLOCKSTACKSIZE};")
        w(f"    {var}.m_functionTable.m_count++;")
        w(f"  }}")
        w("")
        fn_slot[q(fn.name)] = fnslot
        fnslot += 1

    # ── Globals ────────────────────────────────────────────────
    w(f"  // ════ Globals ({len(state.globals)} entries) ════")
    glslot = 0
    for gname, g in state.globals.items():
        slot = glslot
        ref  = f"{var}.m_global.m_data[{slot}]"
        w(f"  // [{slot}] {gname!r}  ({g.type.name} = {g.value})")
        w(f"  {ref}.m_strId = \"{gname}\";")
        if g.type == ParamType._i32:
            w(f"  {ref}.m_data = static_cast<int32_t>({int(g.value)});")
        elif g.type == ParamType._i64:
            w(f"  {ref}.m_data = static_cast<int64_t>({int(g.value)}LL);")
        elif g.type == ParamType._f32:
            w(f"  {ref}.m_data = static_cast<float>({cpp_float(float(g.value))});")
        else:
            w(f"  {ref}.m_data = static_cast<double>({cpp_float(float(g.value))});")
        w(f"  {var}.m_global.m_count++;")
        glslot += 1
    w("")

    # ── Stack pointers ─────────────────────────────────────────
    sp = state.stack_pointer
    if sp:
        w(f"  // ════ Stack / OpStack — $__stack_pointer = {sp} ════")
        for stk in ("m_stack", "m_opStack"):
            w(f"  {var}.{stk}.m_basePointer  = {sp}ULL;")
            w(f"  {var}.{stk}.m_stackPointer = {sp}ULL;")
            w(f"  {var}.{stk}.m_floorPointer = {sp}ULL;")
        w("")

    # ── Memory — only non-zero bytes ───────────────────────────
    mem = state.memory.data
    runs: list[tuple[int,bytes]] = []
    i = 0
    while i < len(mem):
        if mem[i]:
            j = i
            while j < len(mem) and mem[j]: j += 1
            runs.append((i, bytes(mem[i:j]))); i = j
        else:
            i += 1

    if runs:
        w(f"  // ════ Memory ({sum(len(r) for _,r in runs)} non-zero bytes in {len(runs)} region(s)) ════")
        for addr, chunk in runs:
            for off in range(0, len(chunk), 16):
                group = chunk[off:off+16]
                assigns = "  ".join(
                    f"{var}.m_memory.m_data[{addr+off+k}] = 0x{b:02X};"
                    for k, b in enumerate(group))
                w(f"  {assigns}")
        w("")
        w(f"  {var}.m_heap.m_heapPtr = {state.stack_pointer};")
        w("")

    # ── Virtual table ──────────────────────────────────────────
    if state.vtable_entries:
        w(f"  // ════ Virtual Table ({len(state.vtable_entries)} entries) ════")
        for vtidx, fname in state.vtable_entries:
            slot = fn_slot[q(fname)]
            w(f"  {var}.m_virtualTable.m_data[{vtidx}]"
              f" = &{var}.m_functionTable.m_data[{slot}];  // {fname!r}")
        w("")

    # ── File system — copy from namespace-scope wad_data ───────
    if wad_bytes:
        w(f"  // ════ File System ({len(wad_bytes)} bytes from wad_data) ════")
        w(f"  for (int i = 0; i < {len(wad_bytes)}; i++)")
        w(f"    {var}.m_fileSystem.m_data[i] = static_cast<char>(wad_data[i]);")
        w("")

    w(f"  return 0;")
    w("}")
    w("")
    w("// ── Usage ──────────────────────────────────────────────")
    w("//   #include \"parsedState.hpp\"")
    w("//   make_state(state);")
    return "\n".join(W)

# ──────────────────────────────────────────────────────────────
#  Entry point
# ──────────────────────────────────────────────────────────────
def main():
    if len(sys.argv) < 2:
        print("Usage: python wat_parser.py <input.wat> [output.hpp]", file=sys.stderr)
        sys.exit(1)

    with open(sys.argv[1], "r", encoding="utf-8") as f:
        src = f.read()

    output = emit_state(build_state(src))

    if len(sys.argv) > 2:
        with open(sys.argv[2], "w", encoding="utf-8") as f:
            f.write(output)
        print(f"Written to {sys.argv[2]}", file=sys.stderr)
    else:
        print(output)

if __name__ == "__main__":
    main()