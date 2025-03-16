#include "Gameboy.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string_view>

Gameboy::Gameboy()
    : cpu(memory) {

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
  while (true) {
    cpu.fetchOpcode();
    cpu.decodeAndExecute();
  }
}
