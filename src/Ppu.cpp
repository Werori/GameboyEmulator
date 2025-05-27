#include "GameboyMemory.h"
#include "InterruptFlag.h"
#include "Ppu.h"
#include <iostream>
#include <raylib.h>
#include <stdexcept>

PPU::PPU(GameboyMemory &memory) : memory(memory) {};

void PPU::init() {
  SetTargetFPS(60);
  InitWindow(SCREEN_WIDTH * scaleFactor, SCREEN_HEIGHT * scaleFactor,
             "Game Boy Emulator");

  Image image{};
  image.data = framebuffer;
  image.width = SCREEN_WIDTH;
  image.height = SCREEN_HEIGHT;
  image.mipmaps = 1;
  image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

  frameTexture = LoadTextureFromImage(image);
}

void PPU::renderFrame() {
  std::cout << "renderFrame() called" << std::endl;
  UpdateTexture(frameTexture, framebuffer);
}

void PPU::drawFrame() { DrawTexture(frameTexture, 0, 0, WHITE); }

void PPU::close() {
  if (frameTexture.id != 0) {
    UnloadTexture(frameTexture);
  }
  CloseWindow();
}

uint8_t PPU::getLy() { return ly; }

void PPU::tick(int cycles) {
  cycleCounter += cycles;

  uint8_t lcdc = memory.read(lcdControlAddress);

  if (!(lcdc & 0x80)) { // LCD off early out
    std::cout << "Screen is off" << std::endl;
    ly = 0;
    cycleCounter = 0;
    return;
  }

  // Increment scanline when enough cycles passed
  if (cycleCounter >= cyclesPerScanline) {
    cycleCounter -= cyclesPerScanline;
    ly++;
    std::cout << "ly++ ------------ " << (int)ly << std::endl;
  }

  // VBlank interrupt trigger at line 144
  if (ly == startOfVBlank) {
    std::cout << "VBlank" << std::endl;
    memory.requestInterrupt(InterruptFlag::VBlank);
  }

  // Frame ready trigger and reset at line 154 (one past endOfVBlank)
  if (ly > endOfVBlank) {
    std::cout << "end v blank" << std::endl;
    ly = 0;
    frameReady = true;
  }

  // BG disabled fill black (optional, can happen any time)
  if ((lcdc & 0x1)) {
    uint32_t color = bgPalleteData[3];
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
      for (int x = 0; x < SCREEN_WIDTH; x++) {
        framebuffer[y * SCREEN_WIDTH + x] = color;
      }
    }
  }

  // Write LY to memory register
  memory.write(0xff44, ly);
}

uint32_t PPU::colorToUint32(Color c) {
  return ((uint32_t)c.a << 24) | ((uint32_t)c.r << 16) | ((uint32_t)c.g << 8) |
         ((uint32_t)c.b);
}
