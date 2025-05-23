#include "GameboyMemory.h"
#include "InterruptFlag.h"
#include "Ppu.h"
#include <iostream>
#include <raylib.h>

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
}

void PPU::renderFrame() { UpdateTexture(frameTexture, framebuffer); }

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

  if (cycleCounter >= cyclesPerScanline) {
    cycleCounter -= cyclesPerScanline;
    ly++;
  }

  if (ly == startOfVBlank) {
    std::cout << "VBlank" << std::endl;
    memory.requestInterrupt(InterruptFlag::VBlank);
  }

  if (ly > endOfVBlank) {
    std::cout << "end v blank" << std::endl;
    ly = 0;
    frameReady = true;
  }

  // update ly
  memory.write(0xff44, ly);
}
