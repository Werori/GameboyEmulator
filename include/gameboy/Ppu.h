#pragma once

#include "GameboyMemory.h"
#include <InterruptController.h>
#include <raylib.h>

#include <cstdint>

struct PPU {

  static constexpr uint8_t NUMBER_OF_COLORS_IN_PALLETE = 4;
  GameboyMemory &memory;
  // HardwareRegisters hardwareRegisters;
  uint32_t bgPalleteData[NUMBER_OF_COLORS_IN_PALLETE] = {
      colorToUint32(WHITE), colorToUint32(LIGHTGRAY), colorToUint32(DARKGRAY),
      colorToUint32(BLACK)};
  uint8_t obPallete0Data[NUMBER_OF_COLORS_IN_PALLETE];
  uint8_t obPallete1Data[NUMBER_OF_COLORS_IN_PALLETE];
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
  static constexpr int TILES_IN_ROW = 32;
  static constexpr int TILE_SIZE = 8;
  static constexpr int TILE_BYTES = 16;
  static constexpr int SIGNED_CENTER = 0x9000;
  static constexpr int BYTES_PER_ROW = 2;
  static constexpr int MSB_INDEX = 7;
  static constexpr int WINDOW_X_START_POS = 7;
  static constexpr int MODE_2 = 80;
  static constexpr int MODE_3 = 252;

  uint8_t prevMode = 0;

  uint32_t framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT] = {0};
  Texture2D frameTexture = {0, 0, 0, 0, 0};
  uint8_t scaleFactor = 2;
  uint8_t startOfVBlank = 144;
  uint8_t endOfVBlank = 153;
  int cycleCounter = 0;
  int cyclesPerScanline = 456;

  uint16_t lcdControlAddress = 0xff40;

  uint32_t colorToUint32(Color c);

  void renderBackground();

  void renderWindow();
};
