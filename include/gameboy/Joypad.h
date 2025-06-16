#pragma once

#include "GameboyMemory.h"
#include <cstdint>

class Joypad : public IRegisterObserver {
public:
  Joypad(GameboyMemory &memory);

  void write(uint8_t byte);
  uint8_t read();

  void update();

  void onRegisterWrite(uint16_t address, uint8_t byte);
  uint8_t onRegisterRead(uint16_t address);

  GameboyMemory &memory;
  uint8_t selectBits = 0x30;
  uint8_t buttonState = memory.defaultKey;
  uint8_t lastState = memory.defaultKey;
};
