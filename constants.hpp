#pragma once
#include <string_view>

static constexpr int MAXMODULES = 8;
static constexpr int BRTABLESIZE = 1024;
static constexpr int BLOCKSIZE = 128;
static constexpr int MAXCHILDREN = 4096;
static constexpr int LOCALINSTRSIZE = 128;
static constexpr int BLOCKSTACKSIZE = 128;
static constexpr int MAXNUMPARAMS = 512;
static constexpr int GCSIZE = 8192;

static constexpr int MAXPARTS = 4096;
static constexpr int MAXFUNCTIONS = 4096;
static constexpr int MAXVIRTUALTABLESIZE = 4096;

static constexpr int STACKSIZE = INT32_MAX;
static constexpr int GLOBALSIZE = 1024;

static constexpr int MEMORYSIZE = INT32_MAX;
static constexpr int SCREENWIDTH = 10;
static constexpr int SCREENHEIGHT = 10;

static constexpr std::string_view CALL = "call";
static constexpr std::string_view MAIN = "$main";