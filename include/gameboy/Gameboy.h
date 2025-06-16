#pragma once

#include "Cpu.h"
#include "GameboyMemory.h"
#include "Joypad.h"
#include "Ppu.h"
#include "Timers.h"
#include <cstddef>
#include <string_view>

struct Gameboy {
  GameboyMemory memory;
  PPU ppu;
  CPU cpu;
  Timers timers;
  Joypad joypad;

  Gameboy();

  bool loadROMFromFile(std::string_view filename);
  void run();
};
