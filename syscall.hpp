#pragma once
#include "state.hpp"
#include "types.hpp"
#include <array>
#include <string_view>
#include <variant>

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
  //   throw "syscall strlen not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// toupper
constexpr STATUS TOUPPER(State &state) {
  //   throw "syscall toupper not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// sprintf
constexpr STATUS SPRINTF(State &state) {
  //   throw "syscall sprintf not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
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
  //   throw "syscall malloc not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
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
  std::string_view value;

  // Env Map
  if (key == "DOOMWADDIR") {
    value = ".";
  } else {
    throw "Value not in environment\n";
  }
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
  //   throw "syscall access not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
}
// puts
constexpr STATUS PUTS(State &state) {
  //   throw "syscall puts not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
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
  //   throw "syscall setbuf not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
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
// strcasecmp
constexpr STATUS STRCASECMP(State &state) {
  //   throw "syscall strcasecmp not implemented\n";
  state.m_instrPointer++;
  return STATUS::ERROR;
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