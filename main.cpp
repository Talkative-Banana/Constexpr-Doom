#include "inspect.hpp"
#include "parser.hpp"
#include "runner.hpp"
#include "state.hpp"
#include "types.hpp"
#include <array>
#include <iostream>
#include <string_view>

#ifdef RUNTIME_MODE
int main() {
  // auto finalBuffer = ParseAndRunNoCheck();
  size_t hash = constexpr_hash("$getenv");
  int64_t h = static_cast<int64_t>(hash % MAXFUNCTIONS);
  std::cout << h << std::endl;
}
#else

constexpr auto finalBuffer = RunNoCheck();
constexpr std::string_view SV(finalBuffer.data(), finalBuffer.size());
auto x = inspect<SV>();

#endif

/*
V => Video
M => Menu
Z => Zone Memory Allocator
R => Render/Refresh
P => PlayLoop
D => MainDoom
I => Implementaion Dependent
S => Sound
HU => Headsup Display
ST => Status Bar
*/