#pragma once
#include <string_view>

static constexpr int MAXMODULES = 8;
static constexpr int BRTABLESIZE = 1024;
static constexpr int BLOCKSIZE = 1024;
static constexpr int MAXCHILDREN = 4096;
static constexpr int LOCALINSTRSIZE = 128;
static constexpr int BLOCKSTACKSIZE = 1024;
static constexpr int MAXNUMPARAMS = 512;
static constexpr int GCSIZE = 8192;
static constexpr int LBUFF = 4096;
static constexpr int FDTABLE = 16;

static constexpr int MAXPARTS = 4096;
static constexpr int MAXFUNCTIONS = 4096;
static constexpr int FILESYSTEMSIZE = 1024 * 1024 * 32; // 32 MB
static constexpr int MAXVIRTUALTABLESIZE = 4096;

static constexpr int STACKSIZE = 1024 * 1024;
static constexpr int GLOBALSIZE = 1024;

static constexpr int MEMORYSIZE = 1024 * 1024 * 64;
static constexpr int SCREENWIDTH = 320;
static constexpr int SCREENHEIGHT = 20;

static constexpr std::string_view CALL = "call";
static constexpr std::string_view MAIN = "$main";
