#pragma once

#include "Cpu.h"
#include "GameboyMemory.h"
#include <cstddef>
#include <string_view>

struct Gameboy {
  GameboyMemory memory;
  CPU cpu;

  Gameboy();

  bool loadROMFromFile(std::string_view filename);
  void run();
};
