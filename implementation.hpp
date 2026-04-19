#pragma once
#include "state.hpp"
#include "syscall.hpp"
#include "types.hpp"
#include <array>
#include <string_view>
#include <variant>

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
// -------- Graphics --------
constexpr STATUS I_READSCREEN(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_UPDATENOBLIT(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_SETPALETTE(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_FINISHUPDATE(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_INITGRAPHICS(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_STARTFRAME(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_STARTTIC(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_GETTIME(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_WAITVBL(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_SYSFUNCERROR(State &state) {
  // optionally consume format args later
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_QUIT(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_INIT(State &state) {
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

constexpr STATUS I_ALLOCLOW(State &state) {
  // Just a zero memory allocator
  Stack &op_stk = state.m_opStack;
  int32_t length = std::get<int32_t>(op_stk.Pop().m_data);

  Data ret{};
  ret.m_data = alloc_zeroed(state, length);
  op_stk.Push(ret);
  state.m_instrPointer++;
  return STATUS::OK;
}

constexpr STATUS I_ZONEBASE(State &state) {
  // Update the value at ptr
  Memory &memory = state.m_memory;
  Stack &op_stk = state.m_opStack;

  int32_t m_data = 6;

  // 1. Pop argument
  int32_t address = std::get<int32_t>(op_stk.Pop().m_data);

  int32_t val = m_data * 1024 * 1024;

  // 3. Write value as 4 bytes (little-endian) into memory
  if (address >= 0 && address + 3 < MEMORYSIZE) {
    memory.m_data[address + 0] = static_cast<uint8_t>((val >> 0) & 0xFF);
    memory.m_data[address + 1] = static_cast<uint8_t>((val >> 8) & 0xFF);
    memory.m_data[address + 2] = static_cast<uint8_t>((val >> 16) & 0xFF);
    memory.m_data[address + 3] = static_cast<uint8_t>((val >> 24) & 0xFF);
  }

  Data data{};
  data.m_data = val;
  op_stk.Push(data);

  return MALLOC(state);
}

constexpr STATUS I_NETCMD(State &state) {
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
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_TACTILE(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_SUBMITSOUND(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
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
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_STOPSOUND(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_REGISTERSONG(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_PLAYSONG(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_GETSFXLUMPNUM(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_STARTSOUND(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_PAUSESONG(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}
constexpr STATUS I_RESUMESONG(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_UPDATESOUNDPARAMS(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_STOPSONG(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_UNREGISTERSONG(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_ERROR(State &state) {
  // Update the value at ptr
  PUTS(state);
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