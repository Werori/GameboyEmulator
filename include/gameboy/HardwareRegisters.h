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
  uint8_t soundChannel2LengthTimer = 0x3f;
  uint8_t soundChannel2VolumeAndEnvelope = 0;
  uint8_t soundChannel2PeriodLow = 0xff;
  uint8_t soundChannel2PeriodHighAndControl = 0xbf;
  uint8_t soundChannel3DACEnable = 0x7f;
  uint8_t soundChannel3LengthTimer = 0xff;
  uint8_t soundChannel3OutputLevel = 0x9f;
  uint8_t soundChannel3PeriodLow = 0xff;
  uint8_t soundChannel3PeriodHighAndControl = 0xbf;
  uint8_t soundChannel4LengthTimer = 0xff;
  uint8_t soundChannel4VolumeAndEnvelope = 0;
  uint8_t soundChannel4FrequencyAndRandom = 0;
  uint8_t soundChannel4Control = 0xbf;
  uint8_t masterVolumeAndVinPanning = 0x77;
  uint8_t soundPanning = 0xf3;
  uint8_t soundOnOff = 0xf1;
  uint16_t storageWaveform;
  uint8_t lcdControl = 0x91;
  uint8_t lcdStatus = 0x85;
  uint8_t viewportYPosition = 0;
  uint8_t viewportXPosition = 0;
  uint8_t lyc = 0;
  uint8_t bgPallete = 0xfc;
  uint8_t objPallete0 = 0;
  uint8_t objPallete1 = 0;
  uint8_t windowY = 0;
  uint8_t windowXPlus7 = 0;
};
