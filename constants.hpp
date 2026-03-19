#pragma once
#include <string_view>

static constexpr int MAXMODULES = 8;
static constexpr int BLOCKSIZE = 128;
static constexpr int MAXCHILDREN = 512;
static constexpr int LOCALINSTRSIZE = 128;
static constexpr int BLOCKSTACKSIZE = 128;
static constexpr int MAXNUMPARAMS = 512;

static constexpr int MAXPARTS = 4096;
static constexpr int MAXFUNCTIONS = 4096;

static constexpr int STACKSIZE = 131072;      // 128KB
static constexpr int MAXGLOBALS = 131072 * 8; // 128KB
// reserved global index for stack pointer
static constexpr int GLOBALSTACKPOINTERLOCATION = 0;

static constexpr std::string_view CALL = "call";
static constexpr std::string_view MAIN = "$main";