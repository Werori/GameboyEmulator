#include "GameboyMemory.h"
#include "InterruptFlag.h"
#include "Timers.h"

Timers::Timers(GameboyMemory &memory) : memory(memory) {}

void Timers::tick(int cycles) {
  // timer enabled
  if (timerControl & 0x04) {
    timerCycles += cycles;

    uint8_t clockSelect = timerControl & 0b11;
    int period;

    switch (clockSelect) {
    case 0:
      period = 1024;
      break;
    case 1:
      period = 16;
      break;
    case 2:
      period = 64;
      break;
    case 3:
      period = 256;
      break;
    }

    while (timerCycles >= period) {
      timerCycles -= period;

      if (timerCounter == 0xff) {
        timerCounter = timerModulo;
        memory.requestInterrupt(InterruptFlag::Timer);
      } else {
        timerCounter++;
      }
    }
  }
}

void Timers::onRegisterWrite(uint16_t address, uint8_t byte) {
  switch (address) {
  case 0xff07:
    uint8_t newTimerControl = byte & 0x07;
    bool oldEnable = timerControl & 0b100;
    bool newEnable = newTimerControl & 0b100;
    uint8_t oldClockSelect = timerControl & 0b11;
    uint8_t newClockSelect = newTimerControl & 0b11;

    timerControl = newTimerControl;

    if (oldClockSelect != newClockSelect || oldEnable != newEnable) {
      timerCycles = 0;
    }
    break;
  }
}
