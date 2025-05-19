#pragma once

#include "HardwareRegisters.h"
#include "InterruptController.h"
#include "InterruptFlag.h"
#include "Range.h"
#include <array>
#include <cstdint>
#include <vector>

struct GameboyMemory : InterruptController {
  static constexpr int ROM_BANK_SIZE = 0x4000;     // 16KB
  static constexpr int VRAM_SIZE = 0x2000;         // 8KB
  static constexpr int EXTERNAL_RAM_SIZE = 0x2000; // 8KB
  static constexpr int WRAM_SIZE = 0x1000;         // 4KB
  static constexpr int OAM_SIZE = 0xA0;            // 160 bytes
  static constexpr int HIGH_RAM_SIZE = 0x7f;       // 127 bytes
  static constexpr int IO_REGISTERS_SIZE = 0x80;   // 128 bytes

  HardwareRegisters hardwareRegisters;

  uint8_t interruptEnable = 0;

  Range romBank0Range = {0x0000, 0x3fff};
  Range romBank1Range = {0x4000, 0x7fff};
  Range vRAMRange = {0x8000, 0x9fff};
  Range externalRAMRange = {0xa000, 0xbfff};
  Range wRam1Range = {0xc000, 0xcfff};
  Range wRam2Range = {0xd000, 0xdfff};
  Range oamRange = {0xfe00, 0xfe9f};
  Range ioRange = {0xff00, 0xff7f};
  Range highRamRange = {0xff80, 0xfffe};

  std::array<uint8_t, ROM_BANK_SIZE> romBank0;
  std::array<uint8_t, ROM_BANK_SIZE> romBank1;
  std::array<uint8_t, VRAM_SIZE> vram;
  std::array<uint8_t, EXTERNAL_RAM_SIZE> externalRam;
  std::array<uint8_t, WRAM_SIZE> wram1;
  std::array<uint8_t, WRAM_SIZE> wram2;
  std::array<uint8_t, OAM_SIZE> oam;
  std::array<uint8_t, HIGH_RAM_SIZE> highRam;
  std::array<uint8_t, IO_REGISTERS_SIZE> ioRegisters;

  GameboyMemory();

  void requestInterrupt(InterruptFlag flag) override;

  void loadROM(const std::vector<uint8_t> &romData);
  std::array<uint8_t, ROM_BANK_SIZE> &getMemoryBank(uint16_t address);

  uint8_t read(uint16_t address);
  void write(uint16_t address, uint8_t byte);

  // helper function to read hardware registers
  uint8_t readIORegister(uint16_t address);
  // helper function to write to hardware registers
  void writeIORegister(uint16_t address, uint8_t byte);
};
