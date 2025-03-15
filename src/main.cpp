// #include "raylib.h"
#include "Gameboy.h"
#include <fstream>
// #include <iomanip>
#include <iostream>

int main() {
  Gameboy gameboy;
  // std::fstream inputFile("tetris.gb", std::ios::binary);
  std::fstream inputFile("games/tetris.gb", std::ios::in | std::ios::binary);

  if (!inputFile) {
    std::cerr << "Error opening file!" << std::endl;
    return 1;
  }

  // Read the binary file byte by byte and display it as hex
  unsigned char byte;
  // while (inputFile.read(reinterpret_cast<char *>(&byte), sizeof(byte))) {
  //   // Print each byte as a two-character hexadecimal number
  //   std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte
  //             << " ";
  // }

  std::cout << std::endl; // Newline at the end for formatting
  inputFile.close();      // Close the file
  return 0;
}
