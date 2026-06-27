#pragma once
#include "state.hpp"
#include "syscall.hpp"
#include "types.hpp"
#include <cstdio>
#include <array>
#include <string_view>


constexpr STATUS HandleCall(State &state, const std::string_view &funcName);

constexpr bool isImplementationCall(std::string_view funcName) {
  return funcName == "$I_ReadScreen" || funcName == "$I_UpdateNoBlit" ||
         funcName == "$I_SetPalette" || funcName == "$I_FinishUpdate" ||
         funcName == "$I_GetTime" || funcName == "$I_InitGraphics" ||
         funcName == "$I_StartFrame" || funcName == "$I_StartTic" ||
         funcName == "$I_SubmitSound" || funcName == "$I_SYSFUNCERROR" ||
         funcName == "$I_Init" || funcName == "$I_NetCmd" ||
         funcName == "$I_InitNetwork" || funcName == "$I_WaitVBL" ||
         funcName == "$I_BaseTiccmd" || funcName == "$I_Quit" ||
         funcName == "$I_Tactile" || funcName == "$I_AllocLow" ||
         funcName == "$I_SetChannels" || funcName == "$I_SetMusicVolume" ||
         funcName == "$I_SoundIsPlaying" || funcName == "$I_StopSound" ||
         funcName == "$I_RegisterSong" || funcName == "$I_PlaySong" ||
         funcName == "$I_GetSfxLumpNum" || funcName == "$I_StartSound" ||
         funcName == "$I_PauseSong" || funcName == "$I_ResumeSong" ||
         funcName == "$I_UpdateSoundParams" || funcName == "$I_StopSong" ||
         funcName == "$I_UnRegisterSong" || funcName == "$I_ZoneBase" ||
         funcName == "$I_Error";
}
constexpr int32_t SCREENS_GLOBAL_ADDR = 459392;

// -------- Graphics --------
constexpr STATUS I_READSCREEN(State &state) {
  Stack &op_stk = state.m_opStack;
  int32_t destPtr = op_stk.Pop().get<int32_t>();

  auto &mem = state.m_memory.m_data;
  int32_t screenBuf = static_cast<int32_t>(mem[SCREENS_GLOBAL_ADDR + 0])
               | (static_cast<int32_t>(mem[SCREENS_GLOBAL_ADDR + 1]) << 8)
               | (static_cast<int32_t>(mem[SCREENS_GLOBAL_ADDR + 2]) << 16)
               | (static_cast<int32_t>(mem[SCREENS_GLOBAL_ADDR + 3]) << 24);

  if (screenBuf != 0 && destPtr >= 0) {
    constexpr int32_t SCREEN_BYTES = SCREENWIDTH * SCREENHEIGHT;
    for (int32_t i = 0; i < SCREEN_BYTES; i++) {
      if (destPtr + i < MEMORYSIZE && screenBuf + i < MEMORYSIZE) {
        mem[destPtr + i] = mem[screenBuf + i];
      }
    }
  }

  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_UPDATENOBLIT(State &state) {
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_SETPALETTE(State &state) {
  Stack &op_stk = state.m_opStack;
  int32_t palettePtr = op_stk.Pop().get<int32_t>();

  for (int i = 0; i < 256; i++) {
    state.m_palette.m_data[i].r = state.m_memory.m_data[palettePtr + i*3 + 0];
    state.m_palette.m_data[i].g = state.m_memory.m_data[palettePtr + i*3 + 1];
    state.m_palette.m_data[i].b = state.m_memory.m_data[palettePtr + i*3 + 2];
  }

  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_FINISHUPDATE(State &state) {
  auto &mem = state.m_memory.m_data;
  int32_t screenBuf = static_cast<int32_t>(mem[SCREENS_GLOBAL_ADDR + 0])
               | (static_cast<int32_t>(mem[SCREENS_GLOBAL_ADDR + 1]) << 8)
               | (static_cast<int32_t>(mem[SCREENS_GLOBAL_ADDR + 2]) << 16)
               | (static_cast<int32_t>(mem[SCREENS_GLOBAL_ADDR + 3]) << 24);

  if (screenBuf == 0) {
    state.m_instrPointer++;
    return STATUS::OK;
  }

  state.m_framesDrawn++;
  state.m_instrPointer++;

  if (state.m_framesDrawn == FRAMES_TO_RUN) {
    // Only render the very last frame
    uint32_t out = 0;
    for (int y = 0; y < SCREENHEIGHT; y++) {
      for (int x = 0; x < SCREENWIDTH; x++) {
        int srcX = x * 320 / SCREENWIDTH;
        int srcY = y * 200 / SCREENHEIGHT;
        uint8_t idx = state.m_memory.m_data[screenBuf + srcY * 320 + srcX];
        auto &c = state.m_palette.m_data[idx];
        int brightness = (c.r + c.g + c.b) / 3;
        state.m_frameBuffer.m_data[out++] = " .:-=+*#%@"[brightness * 10 / 256];
      }
    }
    state.m_frameBuffer.m_framePtr = out;
    return STATUS::ISBAD;
  }

  return STATUS::OK;
}

constexpr STATUS I_INITGRAPHICS(State &state) {
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_STARTFRAME(State &state) {
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr int32_t alloc_zeroed(State &state, int32_t size) {
  int32_t ptr = state.m_heap.m_heapPtr;
  state.m_heap.m_heapPtr += size;
  for (int32_t i = 0; i < size; i++)
    state.m_memory.m_data[ptr + i] = 0;
  return ptr;
}

// -------- Synthetic input injection state (add to State in state.hpp) --------

constexpr int32_t writeEvent(State &state, int32_t type, int32_t data1, int32_t data2, int32_t data3) {
  int32_t evPtr = alloc_zeroed(state, 16); // event_t = 4 x i32 = 16 bytes
  auto &mem = state.m_memory.m_data;

  auto writeI32 = [&](int32_t addr, int32_t val) {
    mem[addr + 0] = static_cast<uint8_t>((val >> 0) & 0xFF);
    mem[addr + 1] = static_cast<uint8_t>((val >> 8) & 0xFF);
    mem[addr + 2] = static_cast<uint8_t>((val >> 16) & 0xFF);
    mem[addr + 3] = static_cast<uint8_t>((val >> 24) & 0xFF);
  };

  writeI32(evPtr + 0, type);   // evtype_t type
  writeI32(evPtr + 4, data1);  // key code
  writeI32(evPtr + 8, data2);  // ascii / same as data1 for keys
  writeI32(evPtr + 12, data3); // unused for keys

  return evPtr;
}

constexpr STATUS postKeyEvent(State &state, int32_t evType, int32_t key) {
  int32_t evPtr = writeEvent(state, evType, key, key, 0);

  Data arg{};
  arg.set(evPtr);
  state.m_opStack.Push(arg);

  STATUS res = HandleCall(state, "$D_PostEvent");
  if (res != STATUS::OK) {
    throw "D_PostEvent call failed";
  }
  return STATUS::OK;
}


constexpr STATUS I_STARTTIC(State &state) {
  constexpr int32_t EV_KEYDOWN = 0;
  constexpr int32_t EV_KEYUP = 1;
  constexpr int32_t KEY_ENTER = 13;

  state.m_startTicCount++;

  // Stage 0->1: New Game (down)
  if (state.m_enterStage == 0) {
    state.m_enterStage = 1;
    return postKeyEvent(state, EV_KEYDOWN, KEY_ENTER);
  }
  // Stage 1->2: New Game (up)
  if (state.m_enterStage == 1) {
    state.m_enterStage = 0;
    return postKeyEvent(state, EV_KEYUP, KEY_ENTER);
  }
  state.m_instrPointer++;
  return STATUS::OK;
}

// constexpr STATUS I_STARTTIC(State &state) {
//   state.m_instrPointer++;
//   return STATUS::OK;
// }

constexpr STATUS I_GETTIME(State &state) {
  Stack &op_stk = state.m_opStack;
  Data ret{};
  ret.set(static_cast<int32_t>(state.m_ticks++));
  op_stk.Push(ret);
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_WAITVBL(State &state) {
  if (state.m_ticks != 1e9) {
    throw "Unimplemented I_WaitVBL";
  }
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_SYSFUNCERROR(State &state) {
  if (state.m_ticks != 1e9) {
    throw "System function error called";
  }
  // optionally consume format args later
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_QUIT(State &state) {
  if (state.m_ticks != 1e9) {
    throw "Quit called";
  }
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_INIT(State &state) {
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_ALLOCLOW(State &state) {
  // Just a zero memory allocator
  Stack &op_stk = state.m_opStack;
  int32_t length = op_stk.Pop().get<int32_t>();

  Data ret{};
  ret.set(static_cast<int32_t>(alloc_zeroed(state, length)));
  op_stk.Push(ret);
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_ZONEBASE(State &state) {
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  // I_ZoneBase(int* size) -> void*
  int32_t sizePtr = op_stk.Pop().get<int32_t>(); // &size output param

  constexpr int32_t ZONE_SIZE = 6 * 1024 * 1024; // 6MB

  // Write zone size into the output param
  memory.m_data[sizePtr + 0] = (ZONE_SIZE >> 0)  & 0xFF;
  memory.m_data[sizePtr + 1] = (ZONE_SIZE >> 8)  & 0xFF;
  memory.m_data[sizePtr + 2] = (ZONE_SIZE >> 16) & 0xFF;
  memory.m_data[sizePtr + 3] = (ZONE_SIZE >> 24) & 0xFF;

  // Allocate zone from heap and return pointer
  int32_t ptr = state.m_heap.m_heapPtr;
  state.m_heap.m_heapPtr += ZONE_SIZE;
  for (int32_t i = 0; i < ZONE_SIZE; i++)
    state.m_memory.m_data[ptr + i] = 0;

  Data ret{};
  ret.set(ptr);
  op_stk.Push(ret);

  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_NETCMD(State &state) {
  if (state.m_ticks != 1e9) {
    throw "Unimplemented I_NetCmd";
  }
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_INITNETWORK(State &state) {
  auto &mem = state.m_memory.m_data;

  int32_t doomcomAddr = state.m_heap.m_heapPtr;
  state.m_heap.m_heapPtr += 128; // sizeof(doomcom_t) with data field

  for (int i = 0; i < 128; i++)
    mem[doomcomAddr + i] = 0;

  auto writeI32 = [&](int32_t addr, int32_t val) {
    mem[addr + 0] = (val >> 0) & 0xFF;
    mem[addr + 1] = (val >> 8) & 0xFF;
    mem[addr + 2] = (val >> 16) & 0xFF;
    mem[addr + 3] = (val >> 24) & 0xFF;
  };

  auto writeI16 = [&](int32_t addr, int16_t val) {
    mem[addr + 0] = (val >> 0) & 0xFF;
    mem[addr + 1] = (val >> 8) & 0xFF;
  };

  writeI32(doomcomAddr + 0, 0x12345678); // id
  writeI16(doomcomAddr + 12, 1);         // numnodes
  writeI16(doomcomAddr + 14, 1);         // ticdup
  writeI16(doomcomAddr + 22, 1);         // episode
  writeI16(doomcomAddr + 24, 1);         // map
  writeI16(doomcomAddr + 26, 2);         // skill
  writeI16(doomcomAddr + 28, 0);         // consoleplayer
  writeI16(doomcomAddr + 30, 1);         // numplayers

  writeI32(152404, doomcomAddr); // doomcom pointer
  writeI32(155824, 0);           // netgame = false

  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_BASETICCMD(State &state) {
  // Return pointer to a zeroed ticcmd_t (8 bytes)
  // Just return heap pointer to zeroed memory
  Stack &op_stk = state.m_opStack;
  int32_t ptr = alloc_zeroed(state, 8);
  Data ret{};
  ret.set(ptr);
  op_stk.Push(ret);
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_TACTILE(State &state) {
  if (state.m_ticks != 1e9) {
    throw "Unimplemented I_Tactile";
  }
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_SUBMITSOUND(State &state) {
  state.m_instrPointer++;
  return STATUS::OK;
}
constexpr STATUS I_SETCHANNELS(State &state) {
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_SETMUSICVOLUME(State &state) {
  // pop ttthe volume param
  Stack &op_stk = state.m_opStack;
  op_stk.Pop();
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_SOUNDISPLAYING(State &state) {
  state.m_opStack.Pop(); // handle
  Data ret{};
  ret.set(int32_t{0}); // not playing
  state.m_opStack.Push(ret);
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_STOPSOUND(State &state) {
  state.m_opStack.Pop(); // handle
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_REGISTERSONG(State &state) {
  Stack &op_stk = state.m_opStack;
  op_stk.Pop(); // pop data pointer - ignore it
  Data ret{};
  ret.set(int32_t{1}); // return dummy handle
  op_stk.Push(ret);
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_PLAYSONG(State &state) {
  state.m_opStack.Pop(); // looping
  state.m_opStack.Pop(); // handle
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_GETSFXLUMPNUM(State &state) {
  state.m_opStack.Pop(); // sfxinfo ptr
  Data ret{};
  ret.set(int32_t{0});
  state.m_opStack.Push(ret);
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_STARTSOUND(State &state) {
  // (origin_x, origin_y, sfxid, vol, sep, pitch, priority) or similar
  // pop all args, return dummy channel handle
  for (int i = 0; i < 5; i++)
    state.m_opStack.Pop();
  Data ret{};
  ret.set(int32_t{1});
  state.m_opStack.Push(ret);
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_PAUSESONG(State &state) {
  state.m_opStack.Pop(); // handle
  state.m_instrPointer++;
  return STATUS::OK;
}
constexpr STATUS I_RESUMESONG(State &state) {
  state.m_opStack.Pop(); // handle
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_UPDATESOUNDPARAMS(State &state) {
  if (state.m_ticks != 1e9) {
    throw "Unimplemented I_UpdateSoundParams";
  }
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_STOPSONG(State &state) {
  state.m_opStack.Pop(); // handle
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_UNREGISTERSONG(State &state) {
  state.m_opStack.Pop(); // handle
  state.m_instrPointer++;
  return STATUS::OK;
}

#ifdef RUNTIME_MODE
STATUS I_ERROR(State &state) {
#else
constexpr STATUS I_ERROR(State &state) {
#endif
  if (state.m_frameBuffer.m_framePtr < FrameBuffer::SCREEN_BYTES) {
    state.m_frameBuffer.m_framePtr = FrameBuffer::SCREEN_BYTES;
  }

  PRINTF(state);

#ifdef RUNTIME_MODE
  std::fprintf(stderr, "\n[I_ERROR] active function: %s\n",
  state.m_activeFunction ? state.m_activeFunction->m_name.data() : "(null)");
  std::fprintf(stderr, "[I_ERROR] instrPointer: %zu\n", state.m_instrPointer);
  std::fprintf(stderr, "[I_ERROR] startTicCount: %d\n", state.m_startTicCount);
  std::fprintf(stderr, "[I_ERROR] framesDrawn: %d\n", state.m_framesDrawn);

  std::fprintf(stderr, "[I_ERROR] WASM call chain:\n");
  Stack &stk = state.m_stack;
  int32_t bp = stk.m_basePointer;
  int depth = 0;
  while (bp > 0 && depth < 20) {
    int32_t bPtr_idx = bp ;
    // int32_t blPtr_idx = bp - 1;
    int32_t iPtr_idx = bp - 2;
    int32_t fPtr_idx = bp - 3;
    if (fPtr_idx < 0) break;

    std::fprintf(stderr, "Active function at crash: %.*s\n",
    (int)state.m_activeFunction->m_name.size(),
    state.m_activeFunction->m_name.data());

    int32_t fPtr = stk.m_data[fPtr_idx].get<int32_t>();
    int32_t iPtr = stk.m_data[iPtr_idx].get<int32_t>();
    // int32_t blPtr = stk.m_data[blPtr_idx].get<int32_t>();
    int32_t bPtr = stk.m_data[bPtr_idx].get<int32_t>();
    if (fPtr > 0 && fPtr < MAXFUNCTIONS) {
      std::fprintf(stderr, "  [%d] funcId=%d iPtr=%d\n", depth, fPtr, iPtr);
    } else {
      std::fprintf(stderr, "  [%d] fPtr=%d iPtr=%d (main/invalid)\n", depth, fPtr, iPtr);
    }

    if (fPtr > 0 && fPtr < MAXFUNCTIONS) {
      auto &f = state.m_functionTable.m_data[fPtr % MAXFUNCTIONS];
      std::fprintf(stderr, "Direct caller name: %.*s\n",
        (int)f.m_name.size(), f.m_name.data());
    }
    bp = bPtr;
    depth++;
  }
#endif

  return STATUS::ISBAD;
}

constexpr STATUS dispatchImplemCall(State &state, std::string_view name) {
  // -------- Graphics --------
  if (name == "$I_ReadScreen")
    return I_READSCREEN(state);
  else if (name == "$I_UpdateNoBlit")
    return I_UPDATENOBLIT(state);
  else if (name == "$I_SetPalette")
    return I_SETPALETTE(state);
  else if (name == "$I_FinishUpdate")
    return I_FINISHUPDATE(state);
  else if (name == "$I_InitGraphics")
    return I_INITGRAPHICS(state);
  else if (name == "$I_StartFrame")
    return I_STARTFRAME(state);
  else if (name == "$I_StartTic")
    return I_STARTTIC(state);

  // -------- Timing --------
  else if (name == "$I_GetTime")
    return I_GETTIME(state);
  else if (name == "$I_WaitVBL")
    return I_WAITVBL(state);

  // -------- Core System --------
  else if (name == "$I_SYSFUNCERROR")
    return I_SYSFUNCERROR(state);
  else if (name == "$I_Quit")
    return I_QUIT(state);
  else if (name == "$I_Init")
    return I_INIT(state);

  // -------- Memory --------
  else if (name == "$I_AllocLow")
    return I_ALLOCLOW(state);
  else if (name == "$I_ZoneBase")
    return I_ZONEBASE(state);

  // -------- Input / Network (stubs) --------
  else if (name == "$I_NetCmd")
    return I_NETCMD(state);
  else if (name == "$I_InitNetwork")
    return I_INITNETWORK(state);
  else if (name == "$I_BaseTiccmd")
    return I_BASETICCMD(state);
  else if (name == "$I_Tactile")
    return I_TACTILE(state);

  // -------- Sound / Music (stubs) --------
  else if (name == "$I_SubmitSound")
    return I_SUBMITSOUND(state);
  else if (name == "$I_SetChannels")
    return I_SETCHANNELS(state);
  else if (name == "$I_SetMusicVolume")
    return I_SETMUSICVOLUME(state);
  else if (name == "$I_SoundIsPlaying")
    return I_SOUNDISPLAYING(state);
  else if (name == "$I_StopSound")
    return I_STOPSOUND(state);
  else if (name == "$I_RegisterSong")
    return I_REGISTERSONG(state);
  else if (name == "$I_PlaySong")
    return I_PLAYSONG(state);
  else if (name == "$I_GetSfxLumpNum")
    return I_GETSFXLUMPNUM(state);
  else if (name == "$I_StartSound")
    return I_STARTSOUND(state);
  else if (name == "$I_PauseSong")
    return I_PAUSESONG(state);
  else if (name == "$I_ResumeSong")
    return I_RESUMESONG(state);
  else if (name == "$I_UpdateSoundParams")
    return I_UPDATESOUNDPARAMS(state);
  else if (name == "$I_StopSong")
    return I_STOPSONG(state);
  else if (name == "$I_UnRegisterSong")
    return I_UNREGISTERSONG(state);
  else if (name == "$I_Error")
    return I_ERROR(state);

  throw "Unknow Implem call";
}