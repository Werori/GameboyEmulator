#include "Gameboy.h"

int main() {
  Gameboy gameboy;
  gameboy.loadROMFromFile("games/instr_timing.gb");
  gameboy.run();
  return 0;
}
