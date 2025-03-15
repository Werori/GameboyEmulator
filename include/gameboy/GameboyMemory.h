#pragma once

#include <array>
#include <cstdint>
#include <vector>

struct GameboyMemory {
  static constexpr int ROM_BANK_SIZE = 0x4000; // 16KB
  static constexpr int VRAM_SIZE = 0x2000;     // 8KB
  static constexpr int WRAM_SIZE = 0x2000;     // 8KB
  static constexpr int OAM_SIZE = 0xA0;        // 160 bytes

  std::array<uint8_t, ROM_BANK_SIZE> romBank0;
  std::array<uint8_t, ROM_BANK_SIZE> romBank1;
  std::array<uint8_t, VRAM_SIZE> vram;
  std::array<uint8_t, WRAM_SIZE> wram;
  std::array<uint8_t, OAM_SIZE> oam;

  GameboyMemory();

  void loadROM(const std::vector<uint8_t> &romData);
};
