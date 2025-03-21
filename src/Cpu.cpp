#include "Cpu.h"
#include "GameboyMemory.h"

#include <array>
#include <cstdint>
#include <functional>
#include <ios>
#include <iostream>
// #include <stdexcept>

CPU::CPU(GameboyMemory &memory) : memory(memory) { initializeOpcodes(); };

void CPU::initializeOpcodes() {
  opcodes[0x00] = {nop, 1, 4};
  opcodes[0x06] = {[this]() { CPU::LDBu8(); }, 2, 8};
  opcodes[0x0e] = {[this]() { CPU::LDCu8(); }, 2, 8};
  opcodes[0x21] = {[this]() { CPU::LDHLu16(); }, 3, 12};
  opcodes[0xaf] = {[this]() { CPU::XORAA(); }, 1, 4};
  opcodes[0xc3] = {[this]() { CPU::JPa16(); }, 3, 12};
};

void CPU::fetchOpcode() {
  opcode = memory.getMemoryBank(registers.PC)[registers.PC];
}
void CPU::decodeAndExecute() {
  std::cout << std::hex << (int)opcode << std::endl;
  if (opcode != 0xcb) {
    opcodes[opcode].func();
  } else {
    std::cout << "Extended opcode!" << std::endl;
    registers.PC += 1;
    opcode = memory.getMemoryBank(registers.PC)[registers.PC];
    extendedOpcodes[opcode].func();
  }

  if (jumped) {
    jumped = false;
  } else {
    registers.PC += opcodes[opcode].length;
  }
}

void CPU::nop() { std::cout << "Nop function" << std::endl; };
// void CPU::LDBCn16() { throw std::runtime_error("Not implemented"); }

void CPU::LDBu8() {
  registers.setB(memory.getMemoryBank(registers.PC)[registers.PC + 1]);
}

void CPU::LDCu8() {
  registers.setC(memory.getMemoryBank(registers.PC)[registers.PC + 1]);
}

void CPU::XORAA() {
  uint8_t result = registers.getA() ^ registers.getA();

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, false);
  registers.setFlag(registers.FLAG_C, false);
}

void CPU::LDHLu16() {
  auto [highByte, lowByte] = getHighByteAndLowByte();
  registers.HL = toLittleEndian(highByte, lowByte);
}

void CPU::JPa16() {

  // std::cout << "pc: " << std::hex << registers.PC << std::endl;
  auto [highByte, lowByte] = getHighByteAndLowByte();

  // std::cout << "high: " << std::hex << (int)highByte << std::endl;

  // std::cout << "low:" << std::hex << (int)lowByte << std::endl;

  registers.PC = CPU::toLittleEndian(highByte, lowByte);
  jumped = true;
};

uint16_t CPU::toLittleEndian(uint8_t highByte, uint8_t lowByte) {
  return (lowByte << 8) | highByte;
}

std::pair<uint8_t, uint8_t> CPU::getHighByteAndLowByte() {
  uint8_t highByte = memory.getMemoryBank(registers.PC)[registers.PC + 1];
  uint8_t lowByte = memory.getMemoryBank(registers.PC)[registers.PC + 2];

  return {highByte, lowByte};
}
