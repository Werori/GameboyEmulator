#pragma once

#include "GameboyMemory.h"
#include <InterruptController.h>
#include <raylib.h>

#include <cstdint>

struct PPU {

  GameboyMemory memory;
  void tick(int cycles);
  uint8_t getLy();
  bool frameReady = false;

  PPU(GameboyMemory &memory);

  void init();
  void close();
  void renderFrame();
  void drawFrame();

private:
  static constexpr int SCREEN_WIDTH = 160;
  static constexpr int SCREEN_HEIGHT = 144;

  uint32_t framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT] = {0};
  Texture2D frameTexture = {0, 0, 0, 0, 0};
  uint8_t scaleFactor = 2;
  uint8_t ly = 0;
  uint8_t startOfVBlank = 144;
  uint8_t endOfVBlank = 153;
  int cycleCounter = 0;
  int cyclesPerScanline = 456;

  uint16_t lcdControlAddress = 0xff40;
};
