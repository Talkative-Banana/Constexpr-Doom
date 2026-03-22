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
  STATUS result = Run();
  std::cout << static_cast<int>(result) << std::endl;
}
#else
constexpr auto finalBuffer = RunAndGetOutputNoCheck();
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