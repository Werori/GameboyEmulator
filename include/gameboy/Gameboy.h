#pragma once

#include "GameboyMemory.h"
#include <cstddef>
#include <string_view>

struct Gameboy {
  GameboyMemory memory;

  Gameboy();

  bool loadROMFromFile(std::string_view filename);
};
