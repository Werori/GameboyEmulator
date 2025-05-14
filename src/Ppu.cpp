#include "Ppu.h"

PPU::PPU() {};

uint8_t PPU::getLy() { return ly; }

void PPU::tick(int cycles) {
  cycleCounter += cycles;

  if (cycleCounter >= cyclesPerScanline) {
    cycleCounter -= cyclesPerScanline;
    ly++;
  }
}
