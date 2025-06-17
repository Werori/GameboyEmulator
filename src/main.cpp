#include "Gameboy.h"

int main() {
  Gameboy gameboy;
  // gameboy.loadROMFromFile("games/01-special.gb");
  // gameboy.loadROMFromFile("games/02-interrupts.gb");
  // gameboy.loadROMFromFile("games/03-op sp,hl.gb");
  // gameboy.loadROMFromFile("games/04-op r,imm.gb");
  // gameboy.loadROMFromFile("games/07-jr,jp,call,ret,rst.gb");
  // gameboy.loadROMFromFile("games/08-misc instrs.gb");
  // gameboy.loadROMFromFile("games/09-op r,r.gb");
  // gameboy.loadROMFromFile("games/08-misc instrs.gb");
  // gameboy.loadROMFromFile("games/08-misc instrs.gb");
  // gameboy.loadROMFromFile("games/09-op r,r.gb");
  // gameboy.loadROMFromFile("games/06-ld r,r.gb");
  // gameboy.loadROMFromFile("games/10-bit ops.gb");
  // gameboy.loadROMFromFile("games/05-op rp.gb");

  // gameboy.loadROMFromFile("games/dmg-acid2.gb");
  // gameboy.loadROMFromFile("games/hello-world.gb");
  // gameboy.loadROMFromFile("games/sml.gb");
  // gameboy.loadROMFromFile("games/interrupt_time.gb");
  // gameboy.loadROMFromFile("games/interrupt_time.gb");
  gameboy.loadROMFromFile("games/tetris.gb");
  // gameboy.loadROMFromFile("07-jr,jp,call,ret,rst.gb");
  gameboy.run();
  return 0;
}
