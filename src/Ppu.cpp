#include "GameboyMemory.h"
#include "InterruptFlag.h"
#include "Ppu.h"
#include <iostream>

PPU::PPU(GameboyMemory &memory) : memory(memory) {};

uint8_t PPU::getLy() { return ly; }

void PPU::tick(int cycles) {
  cycleCounter += cycles;

  if (cycleCounter >= cyclesPerScanline) {
    cycleCounter -= cyclesPerScanline;
    ly++;
  }

  if (ly == startOfVBlank) {
    std::cout << "VBlank" << std::endl;
    memory.requestInterrupt(InterruptFlag::VBlank);
  }

  if (ly > endOfVBlank) {
    std::cout << "end v blank" << std::endl;
    ly = 0;
  }

  // update ly
  memory.write(0xff44, ly);
}
