#include "Gameboy.h"

int main() {
  Gameboy gameboy;
  gameboy.loadROMFromFile("games/02-interrupts.gb");
  // gameboy.loadROMFromFile("games/dmg-acid2.gb");
  // gameboy.loadROMFromFile("games/hello-world.gb");
  // gameboy.loadROMFromFile("games/tetris.gb");
  // gameboy.loadROMFromFile("07-jr,jp,call,ret,rst.gb");
  gameboy.run();
  return 0;
}
