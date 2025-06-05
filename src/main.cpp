#include "Gameboy.h"

int main() {
  Gameboy gameboy;
  gameboy.loadROMFromFile("games/interrupt_time.gb");
  gameboy.run();
  return 0;
}
