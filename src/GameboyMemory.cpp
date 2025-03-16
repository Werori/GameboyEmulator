#include "GameboyMemory.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

GameboyMemory::GameboyMemory() {
  std::fill(romBank0.begin(), romBank0.end(), 0);
  std::fill(romBank1.begin(), romBank1.end(), 0);
  std::fill(vram.begin(), vram.end(), 0);
  std::fill(wram.begin(), wram.end(), 0);
  std::fill(oam.begin(), oam.end(), 0);
}

void GameboyMemory::loadROM(const std::vector<uint8_t> &romData) {
  constexpr size_t BANK_1_START = 2 * ROM_BANK_SIZE;

  std::copy(romData.begin(), romData.begin() + ROM_BANK_SIZE, romBank0.begin());

  if (romData.size() >= BANK_1_START) {
    std::copy(romData.begin() + ROM_BANK_SIZE, romData.begin() + BANK_1_START,
              romBank1.begin());
  }
}

std::array<uint8_t, GameboyMemory::ROM_BANK_SIZE> &
GameboyMemory::getMemoryBank(uint16_t address) {
  uint8_t bankNumber = address / ROM_BANK_SIZE;

  switch (bankNumber) {
  case 0:
    return romBank0;
  default:
    throw std::out_of_range("Invalid bank");
  }
}
