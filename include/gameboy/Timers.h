#pragma once

#include "GameboyMemory.h"
#include <cstdint>

struct Timers : IRegisterObserver {
  GameboyMemory &memory;
  uint8_t timerCounter = 0;
  uint8_t timerModulo = 0;
  uint8_t timerControl = 0xf8;
  int timerCycles = 0;

  Timers(GameboyMemory &memory);

  void onRegisterWrite(uint16_t address, uint8_t byte);
  void tick(int cycles);
};
