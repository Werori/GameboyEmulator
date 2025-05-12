#pragma once

#include <cstdint>
struct HardwareRegisters {
  uint8_t joypad = 0xcf;
  uint8_t serialTransferData = 0;
  uint8_t serialTransferControl = 0x7e;
  uint8_t dividerRegister = 0xab;
  uint8_t timerCounter = 0;
  uint8_t timerModulo = 0;
  uint8_t timerControl = 0xf8;
  uint8_t interruptFlag = 0xe1;
  uint8_t soundChannel1Sweep = 0x80;
  uint8_t soundChannel1LengthTimer = 0xbf;
  uint8_t soundChannel1VolumeAndEnvelope = 0xf3;
  uint8_t soundChannel1PeriodLow = 0xff;
  uint8_t soundChannel1PeriodHighAndControl = 0xbf;
};
