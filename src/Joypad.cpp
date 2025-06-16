#include "Joypad.h"
#include "raylib.h"
#include <iostream>

Joypad::Joypad() : selectBits(0x30), buttonState(0x0f), lastState(0x0f) {}

void Joypad::update() {
  uint8_t newState = 0x0f;

  if (!(selectBits & 0x10)) {
    // direction buttons
    if (IsKeyDown(KEY_RIGHT)) {
      newState &= ~0x01;
      std::cout << "RIGHT KEY PRESSED" << std::endl;
    }
    if (IsKeyDown(KEY_LEFT)) {
      newState &= ~0x02;
      std::cout << "LEFT KEY PRESSED" << std::endl;
    }
    if (IsKeyDown(KEY_UP)) {
      newState &= ~0x04;
      std::cout << "UP KEY PRESSED" << std::endl;
    }
    if (IsKeyDown(KEY_DOWN)) {
      newState &= ~0x08;
      std::cout << "DOWN KEY PRESSED" << std::endl;
    }
  }

  if (!(selectBits & 0x20)) {
    // action buttons
    if (IsKeyDown(KEY_Z)) {
      newState &= ~0x01;
      std::cout << "Z KEY PRESSED" << std::endl;
    }
    if (IsKeyDown(KEY_X)) {
      newState &= ~0x02;
      std::cout << "X KEY PRESSED" << std::endl;
    }
    if (IsKeyDown(KEY_BACKSPACE)) {
      newState &= ~0x04;
      std::cout << "BACKSPACE KEY PRESSED" << std::endl;
    }
    if (IsKeyDown(KEY_ENTER)) {
      newState &= ~0x08;
      std::cout << "ENTER KEY PRESSED" << std::endl;
    }
  }
}
