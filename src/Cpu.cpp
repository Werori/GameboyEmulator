#include "Cpu.h"
#include "GameboyMemory.h"

#include <array>
#include <iostream>

std::array<std::function<void()>, 256> CPU::opcodes = {};
std::array<size_t, 256> CPU::numOfBytes = {};

CPU::CPU(GameboyMemory &memory) : pc(0x100), sp(0x0), memory(memory) {
  initializeOpcodes();
};

void CPU::initializeOpcodes() {
  opcodes[0x00] = nop;
  numOfBytes[0x0] = 1;
};

void CPU::fetchOpcode() { opcode = memory.romBank0[pc]; }
void CPU::decodeAndExecute() { opcodes[opcode](); }

void CPU::nop() { std::cout << "Nop function" << std::endl; };
