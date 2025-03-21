#pragma once

#include <cstdint>

struct Registers {
  uint16_t AF = 0x0;
  uint16_t BC = 0x0;
  uint16_t DE = 0x0;
  uint16_t HL = 0x0;
  uint16_t SP = 0x0;
  uint16_t PC = 0x100;

  static constexpr uint8_t FLAG_Z = 0x80;
  static constexpr uint8_t FLAG_N = 0x40;
  static constexpr uint8_t FLAG_H = 0x20;
  static constexpr uint8_t FLAG_C = 0x10;

  uint8_t getA();
  uint8_t getF();
  void setFlag(uint8_t flag, bool set);
  void setC(uint8_t address);
};
