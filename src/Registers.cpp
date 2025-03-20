#include "Registers.h"

uint8_t Registers::getA() { return (AF & 0xff00) >> 8; }

uint8_t Registers::getF() { return (AF & 0x00ff); }

void Registers::setFlag(uint8_t flag, bool set) {
  if (set) {
    AF = (AF & 0xff00) | (getF() | flag);
  } else {
    AF = (AF & 0xff00) | (getF() & ~flag);
  }
}
