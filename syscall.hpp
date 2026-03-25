#pragma once
#include "state.hpp"
#include "types.hpp"
#include <array>
#include <string_view>
#include <variant>

// helper — constexpr itoa into memory, returns chars written
constexpr int write_int(Memory &mem, int offset, int32_t val) {
  if (val == 0) {
    mem.m_data[offset] = '0';
    return 1;
  }

  std::array<char, 12> tmp;
  int len = 0;
  bool neg = val < 0;
  // careful: INT32_MIN can't be negated, but fine for now
  if (neg)
    val = -val;
  while (val > 0) {
    tmp[len++] = '0' + (val % 10);
    val /= 10;
  }
  if (neg)
    tmp[len++] = '-';

  for (int j = 0; j < len; j++)
    mem.m_data[offset + j] = tmp[len - 1 - j];

  return len;
}
constexpr int count_format_args(const Memory &memory, int32_t fmt_ptr) {
  int count = 0;

  for (int i = fmt_ptr; i < MEMORYSIZE; i++) {
    char c = memory.m_data[i];

    if (c == '\0')
      break;

    if (c != '%')
      continue;

    // Look at next character
    i++;
    if (i >= MEMORYSIZE)
      break;

    char spec = memory.m_data[i];

    if (spec == '\0')
      break;

    // Escaped percent "%%"
    if (spec == '%') {
      continue;
    }

    // Supported specifiers that consume arguments
    if (spec == 'd' || spec == 'i' || spec == 's' || spec == 'c') {
      count++;
      continue;
    }

    // Optional: skip simple width like %10d
    // (Doom usually doesn't use it, but this makes it safer)
    while (spec >= '0' && spec <= '9') {
      i++;
      if (i >= MEMORYSIZE)
        break;
      spec = memory.m_data[i];
    }

    if (spec == 'd' || spec == 'i' || spec == 's' || spec == 'c') {
      count++;
    }

    // else: unknown specifier → ignore safely
  }

  return count;
}
constexpr bool isSystemCall(std::string_view funcName) {
  return funcName == "$strlen" || funcName == "$toupper" ||
         funcName == "$sprintf" || funcName == "$memcpy" ||
         funcName == "$printf" || funcName == "$fopen" ||
         funcName == "$malloc" || funcName == "$strcpy" ||
         funcName == "$getenv" || funcName == "$access" ||
         funcName == "$puts" || funcName == "$fseek" || funcName == "$ftell" ||
         funcName == "$fread" || funcName == "$fclose" ||
         funcName == "$calloc" || funcName == "$exit" ||
         funcName == "$setbuf" || funcName == "$mkdir" || funcName == "$atoi" ||
         funcName == "$putchar" || funcName == "$getchar" ||
         funcName == "$fprintf" || funcName == "$fputc" ||
         funcName == "$fwrite" || funcName == "$memset" ||
         funcName == "$strcmp" || funcName == "$open" || funcName == "$read" ||
         funcName == "$close" || funcName == "$strncpy" ||
         funcName == "$write" || funcName == "$fstat" || funcName == "$feof" ||
         funcName == "$fscanf" || funcName == "$sscanf" ||
         funcName == "$strcasecmp" || funcName == "$strncasecmp" ||
         funcName == "$lseek" || funcName == "$realloc";
}

// strlen
constexpr STATUS STRLEN(State &state) {
  // The strlen() function calculates the length of the string pointed to by s,
  // excluding the terminating null byte ('\0').
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  // 1. Pop argument (pointer to key string)
  int32_t key_ptr = std::get<int32_t>(op_stk.Pop().m_data);

  // 2. Read string from WASM memory
  int idx = 0;
  while (idx + key_ptr < MEMORYSIZE && memory.m_data[key_ptr + idx] != '\0') {
    idx++;
  }

  Data ret{};
  ret.m_data = idx;
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// toupper
constexpr STATUS TOUPPER(State &state) {
  //   throw "syscall toupper not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// sprintf
constexpr STATUS SPRINTF(State &state) {
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  // pop fixed args first
  int32_t fmt_ptr = std::get<int32_t>(op_stk.Pop().m_data);
  int32_t buf_ptr = std::get<int32_t>(op_stk.Pop().m_data);

  // count args from format string

  // Number of varargs
  int vararg_count = count_format_args(memory, fmt_ptr);

  constexpr int MAX_ARGS = 16;
  Data tmp[MAX_ARGS]{};
  // Pop all arguments (reverse order)
  for (int i = 0; i < vararg_count; i++) {
    tmp[i] = op_stk.Pop();
  }

  // Layout after pop:
  // tmp[0]              = last vararg
  // ...
  // tmp[paramCount-3]   = first vararg
  // tmp[paramCount-2]   = fmt
  // tmp[paramCount-1]   = buf

  // Reorder varargs into correct order
  Data varargs[MAX_ARGS]{};
  for (int i = 0; i < vararg_count; i++) {
    varargs[i] = tmp[vararg_count - 1 - i];
  }

  int out_pos = 0;
  int arg_idx = 0;

  for (int i = fmt_ptr; memory.m_data[i] != '\0'; i++) {
    char c = memory.m_data[i];

    // Bounds check
    if (buf_ptr + out_pos >= MEMORYSIZE) {
      throw "Out of memory access";
      return STATUS::ERROR;
    }

    if (c != '%') {
      memory.m_data[buf_ptr + out_pos++] = c;
      continue;
    }

    // Handle '%'
    i++;
    char spec = memory.m_data[i];

    if (spec == '\0')
      break;

    switch (spec) {
    case '%': {
      memory.m_data[buf_ptr + out_pos++] = '%';
      break;
    }

    case 'd':
    case 'i': {
      if (arg_idx >= vararg_count) {
        throw "Too many integral args";
        return STATUS::ERROR;
      }

      int32_t val = std::get<int32_t>(varargs[arg_idx++].m_data);
      out_pos += write_int(memory, buf_ptr + out_pos, val);
      break;
    }

    case 's': {
      if (arg_idx >= vararg_count) {
        throw "Too many string args";
        return STATUS::ERROR;
      }

      int32_t str_ptr = std::get<int32_t>(varargs[arg_idx++].m_data);

      int j = 0;
      while (true) {
        if (str_ptr + j >= MEMORYSIZE) {
          throw "Out of memory access";
          return STATUS::ERROR;
        }

        char ch = memory.m_data[str_ptr + j];
        if (ch == '\0')
          break;

        if (buf_ptr + out_pos >= MEMORYSIZE) {
          throw "Out of memory access";
          return STATUS::ERROR;
        }

        memory.m_data[buf_ptr + out_pos++] = ch;
        j++;
      }
      break;
    }

    case 'c': {
      if (arg_idx >= vararg_count) {
        throw "Too many string args";
        return STATUS::ERROR;
      }

      char ch = static_cast<char>(std::get<int32_t>(varargs[arg_idx++].m_data));

      memory.m_data[buf_ptr + out_pos++] = ch;
      break;
    }

    default: {
      // Graceful fallback: print literally
      if (buf_ptr + out_pos + 1 >= MEMORYSIZE) {
        throw "Out of memory access";
        return STATUS::ERROR;
      }

      memory.m_data[buf_ptr + out_pos++] = '%';
      memory.m_data[buf_ptr + out_pos++] = spec;
      break;
    }
    }
  }

  // Null terminate
  if (buf_ptr + out_pos >= MEMORYSIZE)
    return STATUS::ERROR;
  memory.m_data[buf_ptr + out_pos] = '\0';

  // Return number of characters written (excluding null)
  Data ret{};
  ret.m_data = static_cast<int32_t>(out_pos);
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// memcpy
constexpr STATUS MEMCPY(State &state) {
  //   throw "syscall memcpy not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// printf
constexpr STATUS PRINTF(State &state) {
  //   throw "syscall printf not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// fopen
constexpr STATUS FOPEN(State &state) {
  //   throw "syscall fopen not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// malloc
constexpr STATUS MALLOC(State &state) {
  //  The  malloc()  function allocates size bytes and returns a pointer to the
  //  allocated memory.  The memory is not initialized.  If size is 0, then
  //  malloc() returns either NULL, or a unique pointer value
  // that can later be successfully passed to free().
  Stack &op_stk = state.m_opStack;

  // 1. Pop argument (size of memory to alloc)
  int32_t size = std::get<int32_t>(op_stk.Pop().m_data);

  // TODO: Fix this to get ptr dynamically
  // 2. get a place to save result to
  int32_t ptr = state.m_heap.m_heapPtr;

  // 3. Increase the ptr
  state.m_heap.m_heapPtr += size;

  // 4. Push return value (pointer)
  Data ret{};
  if (size == 0) {
    ret.m_data = int32_t{0};
  } else {
    ret.m_data = ptr;
  }
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// strcpy
constexpr STATUS STRCPY(State &state) {
  //   throw "syscall strcpy not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// getenv
// The getenv() function searches the environment list to find the environment
// variable name, and returns a pointer to the corresponding value string.
constexpr STATUS GETENV(State &state) {
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  // 1. Pop argument (pointer to key string)
  int32_t key_ptr = std::get<int32_t>(op_stk.Pop().m_data);

  // 2. Read string from WASM memory
  int idx = 0;
  while (idx + key_ptr < MEMORYSIZE && memory.m_data[key_ptr + idx] != '\0') {
    idx++;
  }

  std::array<char, 4096> buff = {0};
  for (int i = 0; i < idx; i++) {
    buff[i] = memory.m_data[key_ptr + i];
  }
  std::string_view key(buff.data(), idx);

  //   3. Lookup (minimal example)
  std::string_view value = "";

  // Env Map (map value)

  // 4. Allocate memory in WASM heap
  int len = value.size() + 1;

  // TODO: Fix this to get ptr dynamically
  // get a place to save result to
  int32_t ptr = state.m_heap.m_heapPtr;
  state.m_heap.m_heapPtr += len;

  // 5. Copy into WASM memory
  for (int i = 0; i < len; ++i) {
    memory.m_data[ptr + i] = buff[i];
  }

  // 6. Push return value (pointer)
  Data ret{};
  ret.m_data = ptr;
  op_stk.Push(ret);

  // advance instruction
  state.m_instrPointer++;
  return STATUS::OK;
}
// access
constexpr STATUS ACCESS(State &state) {
  // access() checks whether the calling process can access the file pathname.
  // If pathname is a symbolic link, it is dereferenced.
  // Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  // 1. Pop argument (pointer to key string)
  std::get<int32_t>(op_stk.Pop().m_data);
  std::get<int32_t>(op_stk.Pop().m_data);

  // int32_t flags = std::get<int32_t>(op_stk.Pop().m_data);
  // int32_t pathName = std::get<int32_t>(op_stk.Pop().m_data);

  Data ret{};
  // Grant Permission => 0
  ret.m_data = 1;
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// puts
constexpr STATUS PUTS(State &state) {
  // puts() writes the string s and a trailing newline to stdout.
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;
  FrameBuffer &frameBuffer = state.m_frameBuffer;

  // 1. Pop argument (pointer to key string)
  int32_t key_ptr = std::get<int32_t>(op_stk.Pop().m_data);

  // 2. Read string from WASM memory
  int idx = 0;
  while (idx + key_ptr < MEMORYSIZE && memory.m_data[key_ptr + idx] != '\0') {
    idx++;
  }

  std::array<char, 4096> buff = {0};
  for (int i = 0; i < idx; i++) {
    buff[i] = memory.m_data[key_ptr + i];
    frameBuffer.m_data[frameBuffer.m_framePtr++] = memory.m_data[key_ptr + i];
  }

  // add newline as per standard
  frameBuffer.m_data[frameBuffer.m_framePtr++] = '\n';
  std::string_view key(buff.data(), idx);

  Data ret{};
  ret.m_data = idx;
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// fseek
constexpr STATUS FSEEK(State &state) {
  //   throw "syscall fseek not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// ftell
constexpr STATUS FTELL(State &state) {
  //   throw "syscall ftell not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// fread
constexpr STATUS FREAD(State &state) {
  //   throw "syscall fread not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// fclose
constexpr STATUS FCLOSE(State &state) {
  //   throw "syscall fclose not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// calloc
constexpr STATUS CALLOC(State &state) {
  //   throw "syscall calloc not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// exit
constexpr STATUS EXIT(State &state) {
  //   throw "syscall exit not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// setbuf
constexpr STATUS SETBUF(State &state) {
  // The  three  types  of  buffering  available are unbuffered, block
  // buffered, and line buffered.  When an output stream is unbuffered,
  // information appears on the destination file or terminal as soon as written;
  // when it is block buffered many characters are saved up and written as a
  // block; when it is line buffered characters are saved up until a newline is
  // output or input is read from any stream at‐ tached to a terminal device(
  // typically stdin).The function fflush(3) may be used to force the block out
  // early.(See fclose(3).)
  Stack &op_stk = state.m_opStack;

  // Pop buffer
  op_stk.Pop().m_data;
  // Pop stream
  op_stk.Pop().m_data;

  state.m_instrPointer++;
  return STATUS::OK;
}
// mkdir
constexpr STATUS MKDIR(State &state) {
  //   throw "syscall mkdir not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// atoi
constexpr STATUS ATOI(State &state) {
  //   throw "syscall atoi not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// putchar
constexpr STATUS PUTCHAR(State &state) {
  //   throw "syscall putchar not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// getchar
constexpr STATUS GETCHAR(State &state) {
  //   throw "syscall getchar not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// fprintf
constexpr STATUS FPRINTF(State &state) {
  //   throw "syscall fprintf not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// fputc
constexpr STATUS FPUTC(State &state) {
  //   throw "syscall fputc not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// fwrite
constexpr STATUS FWRITE(State &state) {
  //   throw "syscall fwrite not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// memset
constexpr STATUS MEMSET(State &state) {
  //   throw "syscall memset not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// strcmp
constexpr STATUS STRCMP(State &state) {
  //   throw "syscall strcmp not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// open
constexpr STATUS OPEN(State &state) {
  //   throw "syscall open not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// read
constexpr STATUS READ(State &state) {
  //   throw "syscall read not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// close
constexpr STATUS CLOSE(State &state) {
  //   throw "syscall close not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// strncpy
constexpr STATUS STRNCPY(State &state) {
  //   throw "syscall strncmp not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// write
constexpr STATUS WRITE(State &state) {
  //   throw "syscall write not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// fstat
constexpr STATUS FSTAT(State &state) {
  //   throw "syscall fstat not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// feof
constexpr STATUS FEOF(State &state) {
  //   throw "syscall feof not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// fscanf
constexpr STATUS FSCANF(State &state) {
  //   throw "syscall fscanf not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// sscanf
constexpr STATUS SSCANF(State &state) {
  //   throw "syscall sscanf not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}

constexpr char tolower_ascii(char c) {
  if (c >= 'A' && c <= 'Z')
    return c + ('a' - 'A');
  return c;
}
// strcasecmp
constexpr STATUS STRCASECMP(State &state) {
  // The  strcasecmp()  function performs a byte-by-byte comparison of the
  // strings s1 and s2, ignoring the case of the characters.  It returns an
  // integer less than, equal to, or greater than zero if s1 is
  // found, respectively, to be less than, to match, or be greater than
  // s2.
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  // Pop in reverse order
  int32_t s2_ptr = std::get<int32_t>(op_stk.Pop().m_data);
  int32_t s1_ptr = std::get<int32_t>(op_stk.Pop().m_data);

  int i = 0;

  while (true) {
    char c1 = memory.m_data[s1_ptr + i];
    char c2 = memory.m_data[s2_ptr + i];

    char l1 = tolower_ascii(c1);
    char l2 = tolower_ascii(c2);

    if (l1 != l2) {
      Data ret{};
      ret.m_data = static_cast<int32_t>(l1 - l2);
      op_stk.Push(ret);

      state.m_instrPointer++;
      return STATUS::OK;
    }

    // If both hit null → equal
    if (c1 == '\0') {
      Data ret{};
      ret.m_data = int32_t{0};
      op_stk.Push(ret);
      state.m_instrPointer++;
      return STATUS::OK;
    }
    i++;
  }
}
// strncasecmp
constexpr STATUS STRNCASECMP(State &state) {
  //   throw "syscall strncasecmp not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// lseek
constexpr STATUS LSEEK(State &state) {
  //   throw "syscall lseek not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// realloc
constexpr STATUS REALLOC(State &state) {
  //   throw "syscall realloc not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}

constexpr STATUS dispatchSysCall(State &state, std::string_view name) {
  if (name == "$strlen")
    return STRLEN(state);
  else if (name == "$toupper")
    return TOUPPER(state);
  else if (name == "$sprintf")
    return SPRINTF(state);
  else if (name == "$memcpy")
    return MEMCPY(state);
  else if (name == "$printf")
    return PRINTF(state);
  else if (name == "$fopen")
    return FOPEN(state);
  else if (name == "$malloc")
    return MALLOC(state);
  else if (name == "$strcpy")
    return STRCPY(state);
  else if (name == "$getenv")
    return GETENV(state);
  else if (name == "$access")
    return ACCESS(state);
  else if (name == "$puts")
    return PUTS(state);
  else if (name == "$fseek")
    return FSEEK(state);
  else if (name == "$ftell")
    return FTELL(state);
  else if (name == "$fread")
    return FREAD(state);
  else if (name == "$fclose")
    return FCLOSE(state);
  else if (name == "$calloc")
    return CALLOC(state);
  else if (name == "$exit")
    return EXIT(state);
  else if (name == "$setbuf")
    return SETBUF(state);
  else if (name == "$mkdir")
    return MKDIR(state);
  else if (name == "$atoi")
    return ATOI(state);
  else if (name == "$putchar")
    return PUTCHAR(state);
  else if (name == "$getchar")
    return GETCHAR(state);
  else if (name == "$fprintf")
    return FPRINTF(state);
  else if (name == "$fputc")
    return FPUTC(state);
  else if (name == "$fwrite")
    return FWRITE(state);
  else if (name == "$memset")
    return MEMSET(state);
  else if (name == "$strcmp")
    return STRCMP(state);
  else if (name == "$open")
    return OPEN(state);
  else if (name == "$read")
    return READ(state);
  else if (name == "$close")
    return CLOSE(state);
  else if (name == "$strncpy")
    return STRNCPY(state);
  else if (name == "$write")
    return WRITE(state);
  else if (name == "$fstat")
    return FSTAT(state);
  else if (name == "$feof")
    return FEOF(state);
  else if (name == "$fscanf")
    return FSCANF(state);
  else if (name == "$sscanf")
    return SSCANF(state);
  else if (name == "$strcasecmp")
    return STRCASECMP(state);
  else if (name == "$strncasecmp")
    return STRNCASECMP(state);
  else if (name == "$lseek")
    return LSEEK(state);
  else if (name == "$realloc")
    return REALLOC(state);

  throw "Unknown syscall";
}