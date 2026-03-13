#pragma once
#include <string_view>

static constexpr int MAXMODULES = 8;
static constexpr int MAXCHILDREN = 512;

static constexpr int MAXPARTS = 4096;
static constexpr int MAXFUNCTIONS = 4096;

static constexpr int STACKSIZE = 131072; // 128KB

static constexpr std::string_view CALL = "call";
static constexpr std::string_view MAIN = "$main";