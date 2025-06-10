#include "Gameboy.h"

int main() {
  Gameboy gameboy;
  gameboy.loadROMFromFile("games/hello-world.gb");
  // gameboy.loadROMFromFile("games/tetris.gb");
  gameboy.run();
  return 0;
}
