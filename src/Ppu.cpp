#include "GameboyMemory.h"
#include "InterruptFlag.h"
#include "Ppu.h"

PPU::PPU(GameboyMemory &memory) : memory(memory) {};

uint8_t PPU::getLy() { return ly; }

void PPU::tick(int cycles) {
  cycleCounter += cycles;

  if (cycleCounter >= cyclesPerScanline) {
    cycleCounter -= cyclesPerScanline;
    ly++;
  }

  if (ly > 153) {
    ly = 0;
  }

  if (ly == 144) {
    memory.requestInterrupt(InterruptFlag::VBlank);
  }

  memory.write(0xff44, ly);
}
