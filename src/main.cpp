#include "Gameboy.h"

int main() {
  Gameboy gameboy;
  gameboy.loadROMFromFile("games/tetris.gb");
  gameboy.run();
  return 0;
}
