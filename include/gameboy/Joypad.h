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

private:
  GameboyMemory &memory;

  uint8_t selectBits;
  uint8_t buttonState;
  uint8_t lastState;

  uint8_t pollButtons();
};
