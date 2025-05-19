#pragma once

#include "GameboyMemory.h"
#include <InterruptController.h>

#include <cstdint>

struct PPU {

  GameboyMemory memory;
  void tick(int cycles);
  uint8_t getLy();

  PPU(GameboyMemory &memory);

private:
  uint8_t ly = 0;
  uint8_t startOfVBlank = 144;
  uint8_t endOfVBlank = 153;
  int cycleCounter = 0;
  int cyclesPerScanline = 456;
};
