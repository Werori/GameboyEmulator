#pragma once

#include "GameboyMemory.h"
#include <cstdint>
class Joypad {
public:
  Joypad(GameboyMemory &memory);

  void write(uint8_t byte);
  uint8_t read();

  void update();

private:
  GameboyMemory &memory;

  uint8_t selectBits;
  uint8_t buttonState;
  uint8_t lastState;

  uint8_t pollButtons();
};
