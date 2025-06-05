#include "Cpu.h"
#include "Gameboy.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <string_view>

Gameboy::Gameboy() : memory(), ppu(memory), cpu(memory, ppu) {
  memory.addObserver(&ppu);
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
  const int CYCLES_PER_FRAME = 70224;
  ppu.init();

  while (!WindowShouldClose()) {
    // Run CPU instructions until enough cycles for one frame
    while (cpu.cycles < CYCLES_PER_FRAME) {
      cpu.fetchOpcode();
      cpu.decodeAndExecute();
    }

    // Now that we've emulated one frame worth of CPU cycles, draw the frame
    BeginDrawing();
    if (ppu.frameReady) {
      ppu.renderFrame();
      ppu.frameReady = false;
    }
    ClearBackground(WHITE);
    ppu.drawFrame();
    EndDrawing();
    cpu.cycles = 0; // used by last opcode
  }

  ppu.close();
}
