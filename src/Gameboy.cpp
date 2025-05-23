#include "Gameboy.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <string_view>

Gameboy::Gameboy()
    : memory(), ppu(memory), cpu(memory, ppu) {

      };

bool Gameboy::loadROMFromFile(std::string_view filename) {
  std::ifstream file(filename.data(), std::ios::binary);

  if (!file) {
    std::cerr << "Invalid ROM file!" << std::endl;
    return false;
  }

  std::vector<uint8_t> romData((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());

  std::cout << romData.size() << std::endl;

  file.close();

  memory.loadROM(romData);

  return true;
}

void Gameboy::run() {
  ppu.init();
  while (!WindowShouldClose()) {
    cpu.fetchOpcode();
    cpu.decodeAndExecute();

    BeginDrawing();
    if (ppu.frameReady) {
      ppu.renderFrame();
      ppu.drawFrame();
      ppu.frameReady = false;
    }
    ClearBackground(RAYWHITE);
    EndDrawing();
  }

  ppu.close();
}
