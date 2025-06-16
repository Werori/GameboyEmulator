#include "GameboyMemory.h"
#include "InterruptFlag.h"
#include "Joypad.h"
#include "raylib.h"
#include <iostream>

Joypad::Joypad(GameboyMemory &memory)
    : memory(memory), selectBits(0x30), buttonState(0x0f), lastState(0x0f) {}

void Joypad::write(uint8_t value) { selectBits = value & 0x30; }

uint8_t Joypad::read() {
  uint8_t result = 0xCF;

  result &= ~0x30;
  result |= selectBits;

  if (!(selectBits & 0x10)) {
    // Direction buttons selected
    result &= (buttonState >> 4) | 0xF0;
  } else if (!(selectBits & 0x20)) {
    // Action buttons selected
    result &= (buttonState & 0x0F) | 0xF0;
  }

  // return result;
  return result;
}

void Joypad::update() {
  uint8_t newState = 0xcf;

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

  if ((buttonState & ~newState) != 0) {
    memory.requestInterrupt(InterruptFlag::Joypad);
  }

  lastState = buttonState;
  buttonState = newState;
}

void Joypad::onRegisterWrite(uint16_t address, uint8_t byte) {
  switch (address) {
  case 0xff00:
    write(byte);
    break;
  }
}

uint8_t Joypad::onRegisterRead(uint16_t address) {
  switch (address) {
  case 0xff00:
    return read();
    break;
  }
  return memory.defaultKey;
}
