#include "Cpu.h"
#include "Gameboy.h"
#include "Joypad.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <string_view>

Gameboy::Gameboy()
    : memory(), ppu(memory), cpu(memory, ppu, timers), timers(memory),
      joypad(memory) {
  memory.addObserver(&joypad);
  memory.addObserver(&ppu);
  memory.addObserver(&timers);
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
  cpu.openLogFile("cpu_log.txt");
  const int CYCLES_PER_FRAME = 70224;
  ppu.init();

  while (!WindowShouldClose()) {
    while (cpu.cycles < CYCLES_PER_FRAME) {
      cpu.logCPUState();
      if (cpu.halted) {
        uint8_t interruptFlag = memory.read(0xFF0F);
        uint8_t interruptEnable = memory.read(0xFFFF);
        uint8_t firedInterrupts = interruptFlag & interruptEnable;

        if (firedInterrupts) {
          cpu.halted = false;
        }
        ppu.tick(1);
        timers.tick(1);
      } else {
        joypad.update();
        cpu.fetchOpcode();
        cpu.decodeAndExecute();
      }
    }
    BeginDrawing();
    ClearBackground(WHITE);

    if (ppu.frameReady) {
      ppu.renderFrame();
      ppu.frameReady = false;
    }
    ppu.drawFrame();
    EndDrawing();
    cpu.cycles = 0;
  }

  ppu.close();
  cpu.closeLogFile();
}
