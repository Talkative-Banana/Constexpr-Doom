#pragma once
#include "state.hpp"
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
         funcName == "$I_UnRegisterSong" || funcName == "$I_ZoneBase";
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
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_ALLOCLOW(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_ZONEBASE(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_NETCMD(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_INITNETWORK(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
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
  return STATUS::SYSFUNCERROR;
}

constexpr STATUS I_SETMUSICVOLUME(State &state) {
  state.m_instrPointer++;
  return STATUS::SYSFUNCERROR;
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

  throw "Unknow Implem call";
}