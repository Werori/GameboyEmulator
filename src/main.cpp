#include "Gameboy.h"

int main() {
  Gameboy gameboy;
  gameboy.loadROMFromFile("games/cpu_instrs.gb");
  gameboy.run();
  return 0;
}
