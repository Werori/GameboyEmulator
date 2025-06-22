#include "Gameboy.h"

int main() {
  Gameboy gameboy;
  // gameboy.loadROMFromFile("games/tetris.gb");
  gameboy.loadROMFromFile("games/09-op r,r.gb");
  gameboy.run();
  return 0;
}
