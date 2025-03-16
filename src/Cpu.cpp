#include "Cpu.h"

#include <array>
#include <iostream>

std::array<std::function<void()>, 256> CPU::opcodes = {};
std::array<size_t, 256> CPU::numOfBytes = {};

CPU::CPU() : pc(0x100), sp(0x0) { initializeOpcodes(); };

void CPU::initializeOpcodes() {
  opcodes[0x00] = nop;
  numOfBytes[0x0] = 1;
};

void CPU::nop() { std::cout << numOfBytes[0x0] << std::endl; };
