#pragma once
#include <string_view>

static constexpr int MAXMODULES = 8;
static constexpr int BRTABLESIZE = 512;
static constexpr int BLOCKSIZE = 512;
static constexpr int MAXCHILDREN = 800;
static constexpr int LOCALINSTRSIZE = 128;
static constexpr int BLOCKSTACKSIZE = 512;
static constexpr int MAXNUMPARAMS = 32;
static constexpr int GCSIZE = 1024;
static constexpr int LBUFF = 4096;
static constexpr int FDTABLE = 16;

static constexpr int MAXPARTS = 800;
static constexpr int MAXFUNCTIONS = 800;
// 600KB - tune down if WAD fits smaller
static constexpr int FILESYSTEMSIZE = 1024 * 600;
static constexpr int MAXVIRTUALTABLESIZE = 700;

static constexpr int STACKSIZE = 1024 * 528;
static constexpr int GLOBALSIZE = 650;

// 9 WASM pages = 576KB, small headroom
static constexpr int MEMORYSIZE = 1024 * 1204 * 8;
static constexpr int SCREENWIDTH = 320;
static constexpr int SCREENHEIGHT = 200;

static constexpr std::string_view CALL = "call";
static constexpr std::string_view MAIN = "$main";