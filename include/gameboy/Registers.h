#pragma once

#include <cstdint>

struct Registers {
  uint16_t AF = 0x01b0;
  uint16_t BC = 0x0013;
  uint16_t DE = 0x00d8;
  uint16_t HL = 0x014d;
  uint16_t SP = 0xfffe;
  uint16_t PC = 0x0100;

  static constexpr uint8_t FLAG_Z = 0x80;
  static constexpr uint8_t FLAG_N = 0x40;
  static constexpr uint8_t FLAG_H = 0x20;
  static constexpr uint8_t FLAG_C = 0x10;

  uint8_t getA();
  void setA(uint8_t address);

  uint8_t getF();
  void setFlag(uint8_t flag, bool set);

  void setC(uint8_t address);
  uint8_t getC();

  void setB(uint8_t address);
  uint8_t getB();

  bool isFlagSet(uint8_t flag);

  uint8_t getE();
  void setE(uint8_t address);

  uint8_t getL();
  void setL(uint8_t address);

  uint8_t getH();
  void setH(uint8_t address);
};
