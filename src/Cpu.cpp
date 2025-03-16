#include "Cpu.h"
#include "GameboyMemory.h"

#include <array>
#include <iostream>
#include <stdexcept>

CPU::CPU(GameboyMemory &memory) : memory(memory) { initializeOpcodes(); };

void CPU::initializeOpcodes() {
  opcodes[0x00] = {nop, 1, 4};
  opcodes[0x01] = {LDBCn16, 3, 12};
};

void CPU::fetchOpcode() { opcode = memory.romBank0[registers.PC]; }
void CPU::decodeAndExecute() {
  opcodes[opcode].func();
  registers.PC += opcodes[opcode].length;
}

void CPU::nop() { std::cout << "Nop function" << std::endl; };
void CPU::LDBCn16() { throw std::runtime_error("Not implemented"); }
