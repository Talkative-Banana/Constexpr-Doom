#pragma once
#include "state.hpp"
#include "types.hpp"
#include <array>
#include <string_view>

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
// helper — constexpr itoa into frame buffer, returns chars written
constexpr int write_int(FrameBuffer &frameBuffer, int32_t val) {
  if (val == 0) {
    frameBuffer.m_data[frameBuffer.m_framePtr] = '0';
    frameBuffer.m_framePtr++;
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

  for (int j = 0; j < len; j++) {
    frameBuffer.m_data[frameBuffer.m_framePtr++] = tmp[len - 1 - j];
  }

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
  int32_t key_ptr = op_stk.Pop().get<int32_t>();

  // 2. Read string from WASM memory
  int idx = 0;
  while (idx + key_ptr < MEMORYSIZE && memory.m_data[key_ptr + idx] != '\0') {
    idx++;
  }

  Data ret{};
  ret.set(idx);
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// toupper
constexpr STATUS TOUPPER(State &state) {
  // These function convert lowecase letters to uppercase, and vice versa.
  Stack &op_stk = state.m_opStack;

  // 1. Pop argument (pointer to key character)
  int32_t val = op_stk.Pop().get<int32_t>();

  // 2. Read char from WASM memory and convert to uppercase
  int32_t res = 0;

  char c = val;
  if (c >= 'a' && c <= 'z') {
    res = c - 'a' + 'A';
  } else {
    res = c;
  }

  Data ret{};
  ret.set(res);
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// sprintf
constexpr STATUS SPRINTF(State &state) {
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  if (op_stk.m_vargCount < 2)
    throw "Insufficient arguments for sprintf syscall";

  bool has_varargs = (op_stk.m_vargCount == 3);

  int32_t varargs_ptr =
      has_varargs ? op_stk.Pop().get<int32_t>() : -1;
  int32_t fmt_ptr = op_stk.Pop().get<int32_t>();
  int32_t buf_ptr = op_stk.Pop().get<int32_t>();

  if (fmt_ptr < 0 || fmt_ptr >= MEMORYSIZE)
    throw "Invalid memory pointer for format string";
  if (buf_ptr < 0 || buf_ptr >= MEMORYSIZE)
    throw "Invalid memory pointer for buffer";

  int out_pos = 0;
  int varg_off = 0;

  auto read_vararg = [&](int32_t &out) -> bool {
    if (!has_varargs || varargs_ptr < 0)
      return false;
    if (varargs_ptr + varg_off + 3 >= MEMORYSIZE)
      return false;
    out = static_cast<int32_t>(memory.m_data[varargs_ptr + varg_off]) |
          static_cast<int32_t>(memory.m_data[varargs_ptr + varg_off + 1]) << 8 |
          static_cast<int32_t>(memory.m_data[varargs_ptr + varg_off + 2])
              << 16 |
          static_cast<int32_t>(memory.m_data[varargs_ptr + varg_off + 3]) << 24;
    varg_off += 4;
    return true;
  };

  for (int i = fmt_ptr; i < MEMORYSIZE && memory.m_data[i] != '\0'; i++) {
    char c = memory.m_data[i];

    if (buf_ptr + out_pos >= MEMORYSIZE)
      throw "Buffer overflow in sprintf";

    if (c != '%') {
      memory.m_data[buf_ptr + out_pos++] = c;
      continue;
    }

    ++i;

    // skip flags: -, +, space, 0
    int zero_pad = 0;
    while (memory.m_data[i] == '-' || memory.m_data[i] == '0' ||
           memory.m_data[i] == '+' || memory.m_data[i] == ' ') {
      if (memory.m_data[i] == '0')
        zero_pad = 1;
      i++;
    }

    // parse width
    int width = 0;
    while (memory.m_data[i] >= '0' && memory.m_data[i] <= '9')
      width = width * 10 + (memory.m_data[i++] - '0');

    // parse precision
    int precision = -1;
    if (memory.m_data[i] == '.') {
      i++;
      precision = 0;
      while (memory.m_data[i] >= '0' && memory.m_data[i] <= '9')
        precision = precision * 10 + (memory.m_data[i++] - '0');
    }

    char spec = memory.m_data[i];
    if (spec == '\0')
      break;

    switch (spec) {
    case '%':
      memory.m_data[buf_ptr + out_pos++] = '%';
      break;

    case 'd':
    case 'i': {
      int32_t val = 0;
      if (!read_vararg(val))
        throw "Error reading vararg for sprintf";
      // compute digits
      std::array<char, 12> tmp{};
      int len = 0;
      bool neg = val < 0;
      int32_t uval = neg ? -val : val;
      if (uval == 0)
        tmp[len++] = '0';
      while (uval > 0) {
        tmp[len++] = '0' + (uval % 10);
        uval /= 10;
      }
      // zero pad to precision or width
      int pad = precision >= 0 ? precision : (zero_pad ? width : 0);
      while (len < pad)
        tmp[len++] = '0';
      if (neg)
        tmp[len++] = '-';
      // write reversed
      for (int j = len - 1; j >= 0; j--)
        memory.m_data[buf_ptr + out_pos++] = tmp[j];
      break;
    }

    case 's': {
      int32_t str_ptr = 0;
      if (!read_vararg(str_ptr))
        throw "Error reading string pointer for sprintf";
      if (str_ptr < 0 || str_ptr >= MEMORYSIZE)
        throw "Invalid string pointer for sprintf";
      for (int j = 0; str_ptr + j < MEMORYSIZE; ++j) {
        char ch = memory.m_data[str_ptr + j];
        if (ch == '\0')
          break;
        memory.m_data[buf_ptr + out_pos++] = ch;
      }
      break;
    }

    case 'c': {
      int32_t val = 0;
      if (!read_vararg(val))
        throw "Error reading vararg for sprintf";
      memory.m_data[buf_ptr + out_pos++] = static_cast<char>(val);
      break;
    }

    default:
      memory.m_data[buf_ptr + out_pos++] = '%';
      memory.m_data[buf_ptr + out_pos++] = spec;
      break;
    }
  }

  if (buf_ptr + out_pos >= MEMORYSIZE)
    throw "Buffer overflow in sprintf";
  memory.m_data[buf_ptr + out_pos] = '\0';

  Data ret{};
  ret.set(static_cast<int32_t>(out_pos));
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// memcpy
constexpr STATUS MEMCPY(State &state) {
  // The memcpy() function copies n bytes from memory area src to memory area
  // dest.  The memory areas must not overlap.
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  // 1. Pop arguments (dest, src, n)
  int32_t n = op_stk.Pop().get<int32_t>();
  int32_t src = op_stk.Pop().get<int32_t>();
  int32_t dest = op_stk.Pop().get<int32_t>();

  // 2. Copy bytes in memory
  if (src >= 0 && src + n <= MEMORYSIZE && dest >= 0 &&
      dest + n <= MEMORYSIZE) {
    for (int i = 0; i < n; i++) {
      memory.m_data[dest + i] = memory.m_data[src + i];
    }
  }

  Data ret{};
  ret.set(dest);
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// printf
constexpr STATUS PRINTF(State &state) {
  // Print ARGUMENT(s) according to FORMAT, or execute according to OPTION:
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;
  FrameBuffer &frameBuffer = state.m_frameBuffer;

  if (op_stk.m_vargCount < 1)
    throw "Insufficient arguments for printf syscall";

  bool has_varargs = (op_stk.m_vargCount >= 2);

  int32_t varargs_ptr =
      has_varargs ? op_stk.Pop().get<int32_t>() : -1;
  int32_t fmt_ptr = op_stk.Pop().get<int32_t>();

  if (fmt_ptr < 0 || fmt_ptr >= MEMORYSIZE)
    throw "Invalid memory pointer for format string";

  int out_pos = frameBuffer.m_framePtr;
  int varg_off = 0;

  auto read_vararg = [&](int32_t &out) -> bool {
    if (!has_varargs || varargs_ptr < 0)
      return false;
    if (varargs_ptr + varg_off + 3 >= MEMORYSIZE)
      return false;
    out = static_cast<int32_t>(memory.m_data[varargs_ptr + varg_off]) |
          static_cast<int32_t>(memory.m_data[varargs_ptr + varg_off + 1]) << 8 |
          static_cast<int32_t>(memory.m_data[varargs_ptr + varg_off + 2])
              << 16 |
          static_cast<int32_t>(memory.m_data[varargs_ptr + varg_off + 3]) << 24;
    varg_off += 4;
    return true;
  };

  for (int i = fmt_ptr; i < MEMORYSIZE && memory.m_data[i] != '\0'; i++) {
    char c = memory.m_data[i];

    if (c != '%') {
      frameBuffer.m_data[frameBuffer.m_framePtr++] = c;
      continue;
    }

    ++i;
    char spec = memory.m_data[i];
    if (spec == '\0')
      break;

    switch (spec) {
    case '%':
      frameBuffer.m_data[frameBuffer.m_framePtr++] = '%';
      break;

    case 'd':
    case 'i': {
      int32_t val = 0;
      if (!read_vararg(val))
        throw "Error reading vararg for printf";
      out_pos += write_int(frameBuffer, val);
      break;
    }

    case 's': {
      int32_t str_ptr = 0;
      if (!read_vararg(str_ptr))
        throw "Error reading string pointer for printf";
      if (str_ptr < 0 || str_ptr >= MEMORYSIZE)
        throw "Invalid string pointer for printf";

      for (int j = 0; str_ptr + j < MEMORYSIZE; ++j) {
        char ch = memory.m_data[str_ptr + j];
        if (ch == '\0')
          break;
        frameBuffer.m_data[frameBuffer.m_framePtr++] = ch;
      }
      break;
    }

    case 'c': {
      int32_t val = 0;
      if (!read_vararg(val))
        throw "Error reading vararg for printf";
      frameBuffer.m_data[frameBuffer.m_framePtr++] = static_cast<char>(val);
      break;
    }

    default:
      frameBuffer.m_data[frameBuffer.m_framePtr++] = '%';
      frameBuffer.m_data[frameBuffer.m_framePtr++] = spec;
      break;
    }
  }

  Data ret{};
  ret.set(static_cast<int32_t>(frameBuffer.m_framePtr - out_pos));
  op_stk.Push(ret);
  state.m_instrPointer++;
  return STATUS::OK;
}
// fopen
constexpr STATUS FOPEN(State &state) {
  // The fopen() function opens the file whose name is the string pointed to by
  // pathname and associates a stream with it.
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  // 1. Pop argument (pointer to key string)
  op_stk.Pop().get<int32_t>();
  int32_t address = op_stk.Pop().get<int32_t>();

  int idx = 0;
  while (idx + address < MEMORYSIZE && memory.m_data[address + idx] != '\0') {
    idx++;
  }

  std::array<char, LBUFF> buff = {0};
  for (int i = 0; i < idx; i++) {
    buff[i] = memory.m_data[address + i];
  }

  std::string_view fileName(buff.data(), idx);

  int32_t filePtr = 0;
  // ends-with check
  auto endsWith = [](std::string_view str, std::string_view suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
  };

  if (endsWith(fileName, "waddump.txt")) {
    throw "ye konsi file hai.";
  }

  Data ret{};
  ret.set(filePtr);
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// malloc
constexpr STATUS MALLOC(State &state) {
  //  The  malloc()  function allocates size bytes and returns a pointer to the
  //  allocated memory.  The memory is not initialized.  If size is 0, then
  //  malloc() returns either NULL, or a unique pointer value
  // that can later be successfully passed to free().
  Stack &op_stk = state.m_opStack;

  // 1. Pop argument (size of memory to alloc)
  int32_t size = op_stk.Pop().get<int32_t>();

  // TODO: Fix this to get ptr dynamically [bump alloc for now]
  // 2. get a place to save result to
  int32_t ptr = state.m_heap.m_heapPtr;

  // 3. Increase the ptr
  state.m_heap.m_heapPtr += size;

  // 4. Push return value (pointer)
  Data ret{};
  if (size == 0) {
    ret.set(int32_t{0});
  } else {
    ret.set(ptr);
  }
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// strcpy
constexpr STATUS STRCPY(State &state) {
  //   The  strcpy() function copies the string pointed to by src, including the
  //   terminating null byte ('\0'), to the buffer pointed to by dest.  The
  //   strings may not overlap, and the destination string dest
  //   must be large enough to receive the copy.Beware of buffer
  //   overruns !
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  if (op_stk.m_vargCount < 2) {
    throw "Insufficient arguments for strcpy syscall";
  }

  // 1. Pop argument (pointer to key string)
  int32_t src_ptr = op_stk.Pop().get<int32_t>();
  int32_t dest_ptr = op_stk.Pop().get<int32_t>();

  int idx = 0;
  while (idx + src_ptr < MEMORYSIZE && memory.m_data[src_ptr + idx] != '\0') {
    memory.m_data[dest_ptr + idx] = memory.m_data[src_ptr + idx];
    idx++;
  }
  memory.m_data[dest_ptr + idx] = '\0';

  Data ret{};
  ret.set(dest_ptr);
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// getenv
// The getenv() function searches the environment list to find the environment
// variable name, and returns a pointer to the corresponding value string.
constexpr STATUS GETENV(State &state) {
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  // 1. Pop argument (pointer to key string)
  int32_t key_ptr = op_stk.Pop().get<int32_t>();

  // 2. Read string from WASM memory
  int idx = 0;
  while (idx + key_ptr < MEMORYSIZE && memory.m_data[key_ptr + idx] != '\0') {
    idx++;
  }

  std::array<char, LBUFF> buff = {0};
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
  // GETENV: write null terminator correctly
  memory.m_data[ptr] = '\0';
  for (int i = 0; i < (int)value.size(); ++i) {
    memory.m_data[ptr + i] = static_cast<uint8_t>(value[i]);
  }
  memory.m_data[ptr + value.size()] = '\0';

  // 6. Push return value (pointer)
  Data ret{};
  ret.set(ptr);
  op_stk.Push(ret);

  // advance instruction
  state.m_instrPointer++;
  return STATUS::OK;
}
// access
constexpr STATUS ACCESS(State &state) {
  // access() checks whether the calling process can access the file pathname.
  // If pathname is a symbolic link, it is dereferenced.
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  // 1. Pop argument (pointer to key string)
  op_stk.Pop().get<int32_t>();
  int32_t pathptr = op_stk.Pop().get<int32_t>();

  if (pathptr < 0 || pathptr >= MEMORYSIZE)
    throw "Invalid memory pointer";

  // 2. Read string from WASM memory
  int idx = 0;
  while (idx + pathptr < MEMORYSIZE && memory.m_data[pathptr + idx] != '\0') {
    idx++;
  }

  std::array<char, LBUFF> buff = {0};
  for (int i = 0; i < idx; i++) {
    buff[i] = memory.m_data[pathptr + i];
  }
  std::string_view pathName(buff.data(), idx);

  int32_t permission = 1;

  // ends-with check
  auto endsWith = [](std::string_view str, std::string_view suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
  };

  if (endsWith(pathName, "doom1.wad")) {
    permission = 0; // grant access
  }

  Data ret{};
  // Grant Permission => 0
  ret.set(permission);
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
  int32_t key_ptr = op_stk.Pop().get<int32_t>();

  // 2. Read string from WASM memory
  int idx = 0;
  while (idx + key_ptr < MEMORYSIZE && memory.m_data[key_ptr + idx] != '\0') {
    idx++;
  }

  std::array<char, LBUFF> buff = {0};
  for (int i = 0; i < idx; i++) {
    buff[i] = memory.m_data[key_ptr + i];
    frameBuffer.m_data[frameBuffer.m_framePtr++] = memory.m_data[key_ptr + i];
  }

  // add newline as per standard
  frameBuffer.m_data[frameBuffer.m_framePtr++] = '\n';
  std::string_view key(buff.data(), idx);

  Data ret{};
  ret.set(idx);
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// fseek
constexpr STATUS FSEEK(State &state) {
  //   throw "syscall fseek not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR_FSEEK;
}
// ftell
constexpr STATUS FTELL(State &state) {
  //   throw "syscall ftell not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR_FTELL;
}
// fread
constexpr STATUS FREAD(State &state) {
  //   throw "syscall fread not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR_FREAD;
}
// fclose
constexpr STATUS FCLOSE(State &state) {
  //   throw "syscall fclose not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR_FCLOSE;
}
// calloc
constexpr STATUS CALLOC(State &state) {
  // The  calloc()  function allocates memory for an array of nmemb
  // elements of size bytes each and returns a pointer to the allocated
  // memory.  The memory is set to zero.  If nmemb or size is 0, then
  // calloc() returns either NULL, or a unique pointer value that can later be
  // successfully passed to free().If the multiplication of nmemb and size would
  // result in integer overflow, then calloc() returns an error.By contrast,
  // an integer overflow would not be detected in the following call to
  // malloc(), with the result that an incorrectly sized block of memory would
  // be allocated.
  Stack &op_stk = state.m_opStack;

  // 1. Pop argument (size of memory to alloc)
  int32_t size = op_stk.Pop().get<int32_t>();
  int32_t nmemb = op_stk.Pop().get<int32_t>();

  // TODO: Fix this to get ptr dynamically
  // 2. get a place to save result to
  int32_t ptr = state.m_heap.m_heapPtr;

  // Set to zero
  for (int i = 0; i < size * nmemb; i++) {
    state.m_memory.m_data[i + ptr] = 0;
  }

  // 3. Increase the ptr
  state.m_heap.m_heapPtr += size * nmemb;

  // 4. Push return value (pointer)
  Data ret{};
  if (size * nmemb == 0) {
    ret.set(int32_t{0});
  } else {
    ret.set(ptr);
  }
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// exit
constexpr STATUS EXIT(State &state) {
  //   throw "syscall exit not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR_EXIT;
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
  return STATUS::ERROR_MKDIR;
}
// atoi
constexpr STATUS ATOI(State &state) {
  //   throw "syscall atoi not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR_ATOI;
}
// putchar
constexpr STATUS PUTCHAR(State &state) {
  // putchar(c) is equivalent to putc(c, stdout).
  Stack &op_stk = state.m_opStack;
  FrameBuffer &frameBuffer = state.m_frameBuffer;

  int32_t chr = op_stk.Pop().get<int32_t>();

  frameBuffer.m_data[frameBuffer.m_framePtr++] = chr;
  
  Data ret{};
  ret.set(chr);
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// getchar
constexpr STATUS GETCHAR(State &state) {
  //   throw "syscall getchar not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR_GETCHAR;
}
// fprintf
constexpr STATUS FPRINTF(State &state) {
  // fprintf() and vfprintf() write output to the given output stream
  Stack &op_stk = state.m_opStack;

  // TODO: Implement properly with FILE* handling. For now, just pop the
  // arguments and return 0. Stack: [varargs?], [fmt], [FILE*]
  bool has_varargs = (op_stk.m_vargCount >= 3);
  if (has_varargs) {
    op_stk.Pop(); // varargs
  }
  op_stk.Pop(); // fmt
  op_stk.Pop(); // FILE*

  Data ret{};
  ret.set(int32_t{0});
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// fputc
constexpr STATUS FPUTC(State &state) {
  //   throw "syscall fputc not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR_FPUTC;
}
// fwrite
constexpr STATUS FWRITE(State &state) {
  //   throw "syscall fwrite not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR_FWRITE;
}
// memset
constexpr STATUS MEMSET(State &state) {
  // The memset() function fills the first n bytes of the memory area pointed to
  // by s with the constant byte c.

  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  int32_t size = op_stk.Pop().get<int32_t>();
  int32_t value = op_stk.Pop().get<int32_t>();
  int32_t ptr = op_stk.Pop().get<int32_t>();

  for (int i = 0; i < size; i++) {
    memory.m_data[ptr + i] = static_cast<uint8_t>(value);
  }

  Data ret{};
  ret.set(ptr);
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// strcmp
constexpr STATUS STRCMP(State &state) {
  //   throw "syscall strcmp not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR_STRCMP;
}
// open
constexpr STATUS OPEN(State &state) {
  //   throw "syscall open not implemented\n";
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;
  Descriptor &desc = state.m_descriptor;

  op_stk.Pop().get<int32_t>(); // mode
  op_stk.Pop().get<int32_t>(); // flags
  int32_t filePtr = op_stk.Pop().get<int32_t>();

  // Make all required files available at fixed addresses in memory, and return
  // pointers to them from fopen.
  int idx = 0;
  while (idx + filePtr < MEMORYSIZE && memory.m_data[filePtr + idx] != '\0') {
    idx++;
  }

  std::array<char, LBUFF> buff = {0};
  for (int i = 0; i < idx; i++) {
    buff[i] = memory.m_data[filePtr + i];
  }

  std::string_view fileName(buff.data(), idx);

  int32_t fileDescriptor = -1;

  if (fileName == "/doom1.wad") {
    fileDescriptor = 3; // dummy fd for doom1.wad
  }

  if (fileDescriptor != -1) {
    // Mark the file as opened in the state (if needed)
    desc.m_fdTable[fileDescriptor].m_open = true;
    desc.m_fdTable[fileDescriptor].m_offset = 0;
    // TODO: update size based on actual file size
    desc.m_fdTable[fileDescriptor].m_size = 524779; // size of doom1.wad
    desc.m_fdTable[fileDescriptor].m_dataPtr = 0;
  }

  Data ret{};
  ret.set(int32_t{fileDescriptor});
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// read
constexpr STATUS READ(State &state) {
  // read() attempts to read up to count bytes from file descriptor fd
  // into the buffer starting at buf.

  // On  files that support seeking, the read operation commences at the file
  // offset, and the file offset is incremented by the number of bytes read. If
  // the file offset is at or past the end of file, no bytes are read, and
  // read() returns zero.

  // If count is zero, read() may detect the errors described below.  In the
  // absence of any errors, or if read() does not check for errors, a read()
  // with a count of 0 returns zero and has  no other effects.

  // According to POSIX.1, if count is greater than SSIZE_MAX, the result is
  // implementation-defined; see NOTES for the upper limit on Linux.
  Stack &op_stk = state.m_opStack;
  Memory &memory = state.m_memory;
  FileSystem &fs = state.m_fileSystem;
  Descriptor &desc = state.m_descriptor;

  int32_t count = op_stk.Pop().get<int32_t>();
  int32_t bufPtr = op_stk.Pop().get<int32_t>();
  int32_t handle = op_stk.Pop().get<int32_t>();

  if (handle < 0 || handle >= FDTABLE || !desc.m_fdTable[handle].m_open) {
    Data ret{};
    ret.set(int32_t{-1}); // error
    op_stk.Push(ret);
    state.m_instrPointer++;
    return STATUS::OK;
  }

  int idx = 0;
  while (idx < count && bufPtr + idx < MEMORYSIZE &&
         desc.m_fdTable[handle].m_offset + idx <
             desc.m_fdTable[handle].m_size) {
    memory.m_data[bufPtr + idx] =
        fs.m_data[desc.m_fdTable[handle].m_dataPtr +
                  desc.m_fdTable[handle].m_offset + idx];
    idx++;
  }

  desc.m_fdTable[handle].m_offset += idx;
  Data ret{};
  ret.set(idx); // number of bytes read
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// close
constexpr STATUS CLOSE(State &state) {
  // close()  closes  a  file descriptor, so that it no longer refers to
  // any file and may be reused.  Any record locks (see fcntl(2)) held on
  // the file it was associated with, and owned by the
  // process, are removed(regardless of the file descriptor that was used to
  // obtain the lock).

  //  If fd is the last file descriptor referring to the underlying open file
  //  description(see open(2)), the resources associated with the open file
  //  description are freed; if the file descrip‐ tor was the last reference to
  //  a file which has been removed using unlink(2), the file is deleted.

  Stack &op_stk = state.m_opStack;
  Descriptor &desc = state.m_descriptor;
  int32_t handle = op_stk.Pop().get<int32_t>();

  bool success = false;
  if (handle >= 0 && handle < FDTABLE && desc.m_fdTable[handle].m_open) {
    desc.m_fdTable[handle].m_open = false;
    success = true;
  }

  Data ret{};
  ret.set(int32_t{success}); // success
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// strncpy
constexpr STATUS STRNCPY(State &state) {
  // The strncpy() function is similar,
  // except that at most n bytes of src are copied.Warning
  // : If there is no null byte among the first n bytes of src,
  // the string placed in dest will not be null - terminated.

  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  int32_t n = op_stk.Pop().get<int32_t>();
  int32_t src = op_stk.Pop().get<int32_t>();
  int32_t dst = op_stk.Pop().get<int32_t>();

  int i = 0;
  for (; i < n && memory.m_data[src + i] != '\0'; i++)
    memory.m_data[dst + i] = memory.m_data[src + i];
  for (; i < n; i++) // pad with nulls
    memory.m_data[dst + i] = '\0';

  Data ret{};
  ret.set(dst);
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// write
constexpr STATUS WRITE(State &state) {
  //   throw "syscall write not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR_WRITE;
}
// fstat
constexpr STATUS FSTAT(State &state) {
  //  These  functions  return  information  about  a  file,  in  the
  //  buffer  pointed to by statbuf.  No permissions are required on the
  //  file itself, but—in the case of stat(), fstatat(), and
  //  lstat()—execute(search) permission is required on all of the
  //  directories in pathname that lead to the file.

  //  stat() and fstatat() retrieve information about the file pointed to by
  //  pathname; the differences for fstatat() are described below.

  //  lstat() is identical to stat(), except that if pathname is a symbolic
  //  link, then it returns information about the link itself, not the file
  //  that the link refers to.

  //  fstat() is identical to stat(), except that the file about which
  //  information is to be retrieved is specified by the file descriptor fd.

  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  // Pop in reverse order
  int32_t statPtr = op_stk.Pop().get<int32_t>(); // statPtr
  op_stk.Pop().get<int32_t>(); // handle

  // Write st_size at offset 44 (32-bit Linux stat)
  int32_t size = 524779;
  memory.m_data[statPtr + 0] = (size >> 0) & 0xFF;
  memory.m_data[statPtr + 1] = (size >> 8) & 0xFF;
  memory.m_data[statPtr + 2] = (size >> 16) & 0xFF;
  memory.m_data[statPtr + 3] = (size >> 24) & 0xFF;

  Data ret{};
  // success by default, can set to error code if needed
  ret.set(int32_t{0});
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// feof
constexpr STATUS FEOF(State &state) {
  //   throw "syscall feof not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR_FEOF;
}
// fscanf
constexpr STATUS FSCANF(State &state) {
  //   throw "syscall fscanf not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR_FSCANF;
}
// sscanf
constexpr STATUS SSCANF(State &state) {
  //   throw "syscall sscanf not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR_SSCANF;
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
  int32_t s2_ptr = op_stk.Pop().get<int32_t>();
  int32_t s1_ptr = op_stk.Pop().get<int32_t>();

  int i = 0;

  while (true) {
    char c1 = memory.m_data[s1_ptr + i];
    char c2 = memory.m_data[s2_ptr + i];

    char l1 = tolower_ascii(c1);
    char l2 = tolower_ascii(c2);

    if (l1 != l2) {
      Data ret{};
      ret.set(static_cast<int32_t>(l1 - l2));
      op_stk.Push(ret);

      state.m_instrPointer++;
      return STATUS::OK;
    }

    // If both hit null → equal
    if (c1 == '\0') {
      Data ret{};
      ret.set(int32_t{0});
      op_stk.Push(ret);
      state.m_instrPointer++;
      return STATUS::OK;
    }
    i++;
  }
}
// strncasecmp
constexpr STATUS STRNCASECMP(State &state) {
  // The strncasecmp() function is similar, to strcasecmp(),
  // except that it compares no more than n bytes of s1 and s2.
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  // Pop in reverse order
  int32_t n = op_stk.Pop().get<int32_t>();
  int32_t s2_ptr = op_stk.Pop().get<int32_t>();
  int32_t s1_ptr = op_stk.Pop().get<int32_t>();

  int i = 0;

  while (i < n) {
    char c1 = memory.m_data[s1_ptr + i];
    char c2 = memory.m_data[s2_ptr + i];

    char l1 = tolower_ascii(c1);
    char l2 = tolower_ascii(c2);

    if (l1 != l2) {
      Data ret{};
      ret.set(static_cast<int32_t>(l1 - l2));
      op_stk.Push(ret);

      state.m_instrPointer++;
      return STATUS::OK;
    }

    // If both hit null → equal
    if (c1 == '\0') {
      Data ret{};
      ret.set(int32_t{0});
      op_stk.Push(ret);
      state.m_instrPointer++;
      return STATUS::OK;
    }
    i++;
  }

  Data ret{};
  ret.set(int32_t{0});
  op_stk.Push(ret);
  state.m_instrPointer++;
  return STATUS::OK;
}
// lseek
constexpr STATUS LSEEK(State &state) {
  // lseek() repositions the file offset of the open file description
  // associated with the file descriptor fd to the argument offset
  // according to the directive whence as follows:

  // SEEK_SET The file offset is set to offset bytes.
  // SEEK_CUR The file offset is set to its current location plus offset bytes.
  // SEEK_END The file offset is set to the size of the file plus offset bytes.
  // lseek() allows the file offset to be set beyond the end of the file(but
  // this does not change the size of the file) .If data is later written at
  // this point, subsequent reads of the data in the gap(a "hole") return null
  // bytes('\0') until data is actually written into the gap.

  Stack &op_stk = state.m_opStack;
  Descriptor &desc = state.m_descriptor;

  // Pop in reverse order
  int32_t whence = op_stk.Pop().get<int32_t>();
  int32_t offset = op_stk.Pop().get<int32_t>();
  int32_t handle = op_stk.Pop().get<int32_t>();

  if (handle < 0 || handle >= FDTABLE || !desc.m_fdTable[handle].m_open) {
    Data ret{};
    ret.set(int32_t{-1}); // error
    op_stk.Push(ret);
    state.m_instrPointer++;
    return STATUS::OK;
  }

  FileDesc &fd = desc.m_fdTable[handle];

  if (whence == 0) {
    fd.m_offset = offset;
  } else {
    if (whence == 1) {
      fd.m_offset += offset;
    } else {
      fd.m_offset = fd.m_size + offset;
    }
  }

  Data ret{};
  ret.set(fd.m_offset);
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}
// realloc
constexpr STATUS REALLOC(State &state) {
  // The realloc() function changes the size of the memory block pointed to
  // by ptr to size bytes.  The contents will be unchanged in the range from
  // the start of the region up to  the  minimum
  // of  the  old  and new sizes.  If the new size is larger than the old size,
  // the added memory will not be initialized.  If ptr is NULL, then the call
  // is equivalent to malloc(size), for all values of size;
  // if size is equal to zero, and ptr is not NULL,
  // then the call is equivalent to free(ptr).Unless ptr is NULL,
  // it must have been returned by an earlier call to malloc(), calloc(),
  // or realloc().If the area pointed to was moved, a free(ptr) is done.
  Memory &mem = state.m_memory;
  Stack &op_stk = state.m_opStack;

  int32_t size = op_stk.Pop().get<int32_t>();
  int32_t rPtr = op_stk.Pop().get<int32_t>();

  int32_t newPtr = 0;

  if (rPtr == 0) {
    // NULL ptr — equivalent to malloc(size)
    if (size == 0) {
      Data ret{};
      ret.set(int32_t(0));
      op_stk.Push(ret);
      state.m_instrPointer++;
      return STATUS::OK;
    }
    newPtr = state.m_heap.m_heapPtr;
    state.m_heap.m_heapPtr += size;

    Data ret{};
    ret.set(int32_t(newPtr));
    op_stk.Push(ret);

  } else if (size == 0) {
    // free(ptr) — bump allocator just ignores frees
    Data ret{};
    ret.set(int32_t(0));
    op_stk.Push(ret);
  } else {
    // Allocate new block and copy old contents
    newPtr = state.m_heap.m_heapPtr;
    state.m_heap.m_heapPtr += size;

    // Copy from old ptr (we don't know old size, copy 'size' bytes as best
    // effort)
    for (int32_t i = 0; i < size; i++) {
      mem.m_data[newPtr + i] = mem.m_data[rPtr + i];
    }
    // Old block is "freed" — bump allocator ignores it
    Data ret{};
    ret.set(int32_t(newPtr));
    op_stk.Push(ret);
  }

  state.m_instrPointer++;
  return STATUS::OK;
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
