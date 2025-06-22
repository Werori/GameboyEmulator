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

void PPU::tick(int cycles) {
  cycleCounter = cycleCounter + cycles;
  uint8_t scanlines = memory.getLy();
  uint8_t lcdc = memory.read(lcdControlAddress);

  bool wasOn = (lastLcdc & 0x80);
  bool isOn = (lcdc & 0x80);

  if (wasOn && !isOn) {
    lcdEnabled = false;
    scanlines = 0;
    cycleCounter = 0;
    mode = 0;
  } else if (!wasOn && isOn) {
    lcdEnabled = true;
    cycleCounter = 0;
    mode = 2;
  }

  lastLcdc = lcdc;

  if (!lcdEnabled) {
    scanlines = 0;
    cycleCounter = 0;
    return;
  }

  if (!(lcdc & 0x80)) {
    scanlines = 0;
    cycleCounter = 0;
    return;
  }
  while (cycleCounter >= cyclesPerScanline) {
    scanlines++;
    cycleCounter -= cyclesPerScanline;

    if (scanlines > endOfVBlank) {
      scanlines = 0;
      cycleCounter = 0;
    }

    if (scanlines == startOfVBlank) {
      mode = 1;
      frameReady = true;
      memory.requestInterrupt(InterruptFlag::VBlank);
    } else if (scanlines > startOfVBlank && scanlines <= endOfVBlank) {
      mode = 1;
    } else {
      mode = 2;
    }
  }
  if (scanlines < startOfVBlank) {
    if (cycleCounter < MODE_2) {
      mode = 2;
    } else if (cycleCounter < MODE_3) {
      if (mode != 3) {
        mode = 3;
        if (lcdc & 0x1) {
          // std::cout << "rendering scanlines..." << std::endl;
          renderScanline(scanlines);
          if ((lcdc & 0x20) && scanlines >= memory.getWindowY() &&
              memory.getWindowX() <= SCREEN_WIDTH) {
            std::cout << "lcdc is: " << (int)lcdc << std::endl;
            throw std::runtime_error("window is enabled");
          }
        } else if (lcdc & (1 << 1)) {
          throw std::runtime_error("obj enable");
        }
      }
    } else {
      mode = 0;
    }
  } else {
    mode = 1;
  }
  memory.setLy(scanlines);

  uint8_t stat = memory.read(0xFF41);
  stat = (stat & 0xFC) | (mode & 0x3); // Clear bits 0-1 and set new mode
  memory.write(0xFF41, stat);

  uint8_t lyc = memory.read(0xFF45);
  if (scanlines == lyc) {
    stat |= 0x04;
    if (stat & 0x40) {
      memory.requestInterrupt(InterruptFlag::LCDStat);
    }
  } else {
    stat &= ~0x04;
  }
  memory.write(0xFF41, stat);
  // std::cout << "[PPU] setLy: " << std::hex << (int)ly
  //           << " at cycle: " << cycleCounter << std::endl;
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

uint8_t PPU::onRegisterRead(uint16_t address) {
  switch (address) {
  case 0xff00:
    break;
  }
}

void PPU::onRegisterWrite(uint16_t address, uint8_t byte) {
  switch (address) {
  case 0xff47:
    for (int i = 0; i < 4; ++i) {
      uint8_t shade = (byte >> (i * 2)) & 0x03;
      bgPalleteData[i] = shadeToColor(shade);
    }
    break;
  case 0xff48:
    for (int i = 0; i < 4; ++i) {
      obPallete0Data[i] = (byte >> (i * 2)) & 0x03;
    }
    break;
  case 0xff49:
    for (int i = 0; i < 4; ++i) {
      obPallete1Data[i] = (byte >> (i * 2)) & 0x03;
    }
    break;
  }
}

uint32_t PPU::shadeToColor(uint8_t shade) {
  switch (shade) {
  case 0:
    return colorToUint32(WHITE);
  case 1:
    return colorToUint32(LIGHTGRAY);
  case 2:
    return colorToUint32(DARKGRAY);
  case 3:
    return colorToUint32(BLACK);
  default:
    return 0xffffffff;
  }
}

void PPU::renderScanline(uint8_t ly) {
  uint8_t scrollY = memory.getScy();
  uint8_t scrollX = memory.getScx();
  uint8_t lcdc = memory.read(0xFF40);

  uint16_t tileMapBase = (lcdc & 0x08) ? 0x9C00 : 0x9800;
  bool tileDataSigned = !(lcdc & 0x10);

  uint8_t yInBG = ly + scrollY;
  uint8_t tileRow = yInBG / 8;

  for (int x = 0; x < SCREEN_WIDTH; x++) {
    uint8_t xInBG = x + scrollX;
    uint8_t tileCol = xInBG / 8;

    // Get tile index from background map
    uint16_t tileMapAddr = tileMapBase + tileRow * 32 + tileCol;
    uint8_t tileIndex = memory.read(tileMapAddr);

    // Compute address of tile data
    uint16_t tileAddr;
    if (tileDataSigned)
      tileAddr = 0x9000 + ((int8_t)tileIndex) * TILE_BYTES;
    else
      tileAddr = 0x8000 + tileIndex * TILE_BYTES;

    // Get row inside tile
    uint8_t yInTile = yInBG % 8;
    uint8_t byte1 = memory.read(tileAddr + yInTile * 2);
    uint8_t byte2 = memory.read(tileAddr + yInTile * 2 + 1);

    uint8_t xInTile = 7 - (xInBG % 8);
    uint8_t hi = (byte2 >> xInTile) & 1;
    uint8_t lo = (byte1 >> xInTile) & 1;
    uint8_t colorID = (hi << 1) | lo;

    framebuffer[ly * SCREEN_WIDTH + x] = bgPalleteData[colorID];
  }
}
