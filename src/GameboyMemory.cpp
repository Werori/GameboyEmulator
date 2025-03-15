#include "GameboyMemory.h"

GameboyMemory::GameboyMemory() {
  std::fill(romBank0.begin(), romBank0.end(), 0);
  std::fill(romBank1.begin(), romBank1.end(), 0);
  std::fill(vram.begin(), vram.end(), 0);
  std::fill(wram.begin(), wram.end(), 0);
  std::fill(oam.begin(), oam.end(), 0);
}
