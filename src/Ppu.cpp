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

void PPU::renderFrame() { UpdateTexture(frameTexture, framebuffer); }

void PPU::drawFrame() {
  DrawTexturePro(frameTexture,
                 {0, 0, (float)frameTexture.width, (float)frameTexture.height},
                 {0, 0, static_cast<float>(SCREEN_WIDTH * scaleFactor),
                  static_cast<float>(SCREEN_HEIGHT * scaleFactor)},
                 {0, 0}, 0.0f, WHITE);
}

void PPU::close() {
  if (frameTexture.id != 0) {
    UnloadTexture(frameTexture);
  }
  CloseWindow();
}

uint8_t PPU::getLy() { return memory.getLy(); }

void PPU::tick(int cycles) {
  cycleCounter += cycles;

  uint8_t lcdc = memory.read(lcdControlAddress);

  // check if screen is turned off
  if (!(lcdc & 0x80)) {
    std::cout << "Screen is off" << std::endl;
    memory.setLy(0);
    cycleCounter = 0;
    return;
  }

  // std::cout << "LY: " << (int)memory.getLy() << std::endl;
  // Increment scanline when enough cycles passed
  if (cycleCounter >= cyclesPerScanline) {
    cycleCounter -= cyclesPerScanline;
    memory.setLy(memory.getLy() + 1);
  }

  // VBlank interrupt at line 144
  if (memory.getLy() == startOfVBlank) {
    // std::cout << "VBlank" << std::endl;
    memory.requestInterrupt(InterruptFlag::VBlank);

    frameReady = true;
    if ((lcdc & 0x1)) {
      // std::cout << "BG on" << std::endl;

      renderBackground();

      // std::cout << "lcdc is: " << (int)lcdc << std::endl;
      // std::cout << "ly is: " << (int)memory.getLy() << std::endl;
      // std::cout << "wy:  " << (int)memory.getWindowY() << std::endl;

      // throw std::runtime_error("window is enabled");
      if ((lcdc & 0x20) && (memory.getLy() >= memory.getWindowY())) {
        std::cout << "lcdc is: " << (int)lcdc << std::endl;
        throw std::runtime_error("window is enabled");
      }
    } else if (lcdc & (1 << 1)) {
      throw std::runtime_error("obj enable");
    }
  }

  if (memory.getLy() > endOfVBlank) {
    memory.setLy(0);
  }

  uint8_t stat = memory.read(0xFF41);

  // Write LY to memory register
  memory.write(0xff44, memory.getLy());
  uint8_t ly = memory.getLy();
  int lineCycle = cycleCounter;
  uint8_t mode = 0;
  if (ly >= SCREEN_HEIGHT) {
    mode = 1; // VBlank
  } else if (lineCycle < MODE_2) {
    mode = 2; // OAM
  } else if (lineCycle < MODE_3) {
    mode = 3; // Drawing
  } else {
    mode = 0; // HBlank
  }

  if (mode != prevMode) {
    bool interrupt = false;
    switch (mode) {
    case 0:
      interrupt = stat & (1 << 3);
      break;
    case 1:
      interrupt = stat & (1 << 4);
      break;
    case 2:
      interrupt = stat & (1 << 5);
      break;
    }

    if (interrupt) {
      memory.requestInterrupt(InterruptFlag::LCDStat);
    }

    prevMode = mode;
  }

  stat = (stat & 0xFC) | mode;
  uint8_t lyc = memory.read(0xFF45); // LYC
  if (ly == lyc) {
    stat |= (1 << 2);
    if (stat & (1 << 6)) {
      memory.requestInterrupt(InterruptFlag::LCDStat);
    }
  } else {
    stat &= ~(1 << 2);
  }
  memory.write(0xFF41, stat);
}

uint32_t PPU::colorToUint32(Color c) {
  return ((uint32_t)c.a << 24) | ((uint32_t)c.r << 16) | ((uint32_t)c.g << 8) |
         ((uint32_t)c.b);
}

void PPU::renderBackground() {
  uint8_t lcdc = memory.read(lcdControlAddress);
  uint16_t bgTileMapBase = (lcdc & (1 << 3)) ? 0x9C00 : 0x9800;
  // uint32_t color = bgPalleteData[1];
  // for (int y = 0; y < SCREEN_HEIGHT; y++) {
  //   for (int x = 0; x < SCREEN_WIDTH; x++) {
  //     framebuffer[y * SCREEN_WIDTH + x] = color;
  //   }
  // }
  uint8_t scx = memory.getScx();
  uint8_t scy = memory.getScy();

  for (int screenY = 0; screenY < SCREEN_HEIGHT; screenY++) {
    for (int screenX = 0; screenX < SCREEN_WIDTH; screenX++) {
      // background can be bigger than screen so add scroll values
      int bgX = (screenX + scx) % 256;
      int bgY = (screenY + scy) % 256;

      // find tile in a row and column
      int tileX = bgX / TILE_SIZE;
      int tileY = bgY / TILE_SIZE;

      uint16_t tileMapOffset = tileY * TILES_IN_ROW + tileX;
      uint8_t tileIndex = memory.read(bgTileMapBase + tileMapOffset);
      int tileLine = bgY % TILE_SIZE;

      uint16_t tileAddress;

      if (lcdc & (1 << 4)) {
        tileAddress = 0x8000 + tileIndex * TILE_BYTES;
      } else {
        int8_t signedIndex = static_cast<int8_t>(tileIndex);
        tileAddress = SIGNED_CENTER + signedIndex * 16;
      }

      uint8_t lowByte = memory.read(tileAddress + tileLine * BYTES_PER_ROW);
      uint8_t highByte =
          memory.read(tileAddress + tileLine * BYTES_PER_ROW + 1);
      int tilePixelX = bgX % TILE_SIZE;
      int bitIndex = MSB_INDEX - tilePixelX;
      uint8_t bit0 = (lowByte >> bitIndex) & 1;
      uint8_t bit1 = (highByte >> bitIndex) & 1;
      uint8_t colorId = (bit1 << 1) | bit0;

      framebuffer[screenY * SCREEN_WIDTH + screenX] = bgPalleteData[colorId];
    }
  }
}

void PPU::renderWindow() {
  uint8_t lcdc = memory.read(lcdControlAddress);
  uint16_t windowTileMapBase = (lcdc & (1 << 6)) ? 0x9C00 : 0x9800;

  uint8_t wX = memory.getWindowX() - WINDOW_X_START_POS;
  uint8_t wY = memory.getWindowY();

  for (int windowY = wY; windowY < SCREEN_HEIGHT; windowY++) {
    for (int windowX = wX; windowX < SCREEN_WIDTH; windowX++) {
      int tileX = windowX / TILE_SIZE;
      int tileY = windowY / TILE_SIZE;

      uint16_t tileMapOffset = tileY * TILES_IN_ROW + tileX;
      uint8_t tileIndex = memory.read(windowTileMapBase + tileMapOffset);
      int tileLine = windowY % TILE_SIZE;

      uint16_t tileAddress;

      if (lcdc & (1 << 4)) {
        tileAddress = 0x8000 + tileIndex * TILE_BYTES;
      } else {
        int8_t signedIndex = static_cast<int8_t>(tileIndex);
        tileAddress = SIGNED_CENTER + signedIndex * 16;
      }

      uint8_t lowByte = memory.read(tileAddress + tileLine * BYTES_PER_ROW);
      uint8_t highByte =
          memory.read(tileAddress + tileLine * BYTES_PER_ROW + 1);
      int tilePixelX = windowX % TILE_SIZE;
      int bitIndex = MSB_INDEX - tilePixelX;
      uint8_t bit0 = (lowByte >> bitIndex) & 1;
      uint8_t bit1 = (highByte >> bitIndex) & 1;
      uint8_t colorId = (bit1 << 1) | bit0;

      framebuffer[windowY * SCREEN_WIDTH + windowX] = bgPalleteData[colorId];
    }
  }
}
