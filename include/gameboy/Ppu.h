#pragma once

#include <cstdint>

struct PPU {

  void tick(int cycles);
  uint8_t getLy();

  PPU();

private:
  uint8_t ly = 0;
  int cycleCounter = 0;
  int cyclesPerScanline = 456;
};
