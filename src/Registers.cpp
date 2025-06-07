#include "Registers.h"
#include <cstdint>

uint8_t Registers::getA() { return (AF & 0xff00) >> 8; }

void Registers::setA(uint8_t address) { AF = (address << 8) | (AF & 0x00ff); }

uint8_t Registers::getF() { return (AF & 0x00f0); }

void Registers::setFlag(uint8_t flag, bool set) {
  if (set) {
    AF = (AF & 0xff00) | ((getF() | flag) & 0xf0);
  } else {
    AF = (AF & 0xff00) | ((getF() & ~flag) & 0xf0);
  }
}

void Registers::setC(uint8_t address) { BC = (BC & 0xff00) | address; }

uint8_t Registers::getC() { return BC & 0x00ff; }

void Registers::setB(uint8_t address) { BC = (address << 8) | (BC & 0x00FF); }

uint8_t Registers::getB() { return (BC & 0xff00) >> 8; }

bool Registers::isFlagSet(uint8_t flag) { return (getF() & flag) != 0; }

uint8_t Registers::getE() { return DE & 0x00ff; }

void Registers::setE(uint8_t address) { DE = (DE & 0xff00) | address; }
