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
  auto finalBuffer = RunNoCheck();
  
  // Print pixel region (320x200 ASCII art)
  for (int i = 0; i < 5; i++){
    for (int y = 0; y < SCREENHEIGHT; y++){
      for (int x = 0; x < SCREENWIDTH; x++){
        std::cout << finalBuffer.m_data[i * SCREENHEIGHT * SCREENWIDTH + y * SCREENWIDTH + x];
      }
      std::cout << '\n';
    }
  }


  if (finalBuffer.m_framePtr > FrameBuffer::SCREEN_BYTES) {
    std::cout << "\n--- LOG ---\n";
    for (uint32_t i = FrameBuffer::SCREEN_BYTES; i < finalBuffer.m_framePtr; i++) {
      std::cout << finalBuffer.m_data[i];
    }
    std::cout << '\n';
  }

  return 0;
}
#else

constexpr auto finalBuffer = RunNoCheck();
constexpr std::string_view SV(finalBuffer.m_data.data(), finalBuffer.m_data.size());
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