#include "Cpu.h"
#include "GameboyMemory.h"

#include <array>
#include <cstdint>
#include <functional>
#include <ios>
#include <iostream>
#include <stdexcept>
// #include <stdexcept>

CPU::CPU(GameboyMemory &memory) : memory(memory) { initializeOpcodes(); };

void CPU::initializeOpcodes() {
  opcodes[0x00] = {nop, 1, 4, 4};
  opcodes[0x05] = {[this]() { CPU::DecB(); }, 1, 4, 4};
  opcodes[0x06] = {[this]() { CPU::LDBu8(); }, 2, 8, 8};
  opcodes[0x0d] = {[this]() { CPU::DecC(); }, 1, 4, 4};
  opcodes[0x0e] = {[this]() { CPU::LDCu8(); }, 2, 8, 8};
  opcodes[0x20] = {[this]() { CPU::JRNZi8(); }, 2, 8, 12};
  opcodes[0x21] = {[this]() { CPU::LDHLu16(); }, 3, 12, 12};
  opcodes[0x32] = {[this]() { CPU::LDHLDecA(); }, 1, 8, 8};
  opcodes[0xaf] = {[this]() { CPU::XORAA(); }, 1, 4, 4};
  opcodes[0xc3] = {[this]() { CPU::JPa16(); }, 3, 16, 16};
};

void CPU::fetchOpcode() {
  opcode = memory.read(registers.PC);
  std::cout << "B: " << std::hex << (int)registers.getB() << std::endl;
  std::cout << "A: " << std::hex << (int)registers.getA() << std::endl;
  std::cout << "C: " << std::hex << (int)registers.getC() << std::endl;
  std::cout << "F: " << std::hex << (int)registers.getF() << std::endl;
}
void CPU::decodeAndExecute() {
  std::cout << std::hex << (int)opcode << std::endl;
  if (opcode != 0xcb) {
    opcodes[opcode].func();
  } else {
    std::cout << "Extended opcode!" << std::endl;
    registers.PC += 1;
    opcode = memory.read(registers.PC);
    extendedOpcodes[opcode].func();
  }

  if (jumped) {
    jumped = false;
  } else {
    registers.PC += opcodes[opcode].length;
  }
}

void CPU::nop() { std::cout << "Nop function" << std::endl; };

void CPU::DecB() {
  uint8_t b = registers.getB();

  if (registers.getB() <= 0x10) {
    registers.setFlag(registers.FLAG_H, true);
  } else {
    registers.setFlag(registers.FLAG_H, false);
  }

  registers.setB(b -= 1);

  registers.setFlag(registers.FLAG_Z, registers.getB() == 0);
  registers.setFlag(registers.FLAG_N, true);

  std::cout << std::hex << "FLAG H: " << (int)registers.FLAG_H << std::endl;
  // throw std::runtime_error("stop from decB!");
}

void CPU::LDBu8() { registers.setB(memory.read(registers.PC + 1)); }

void CPU::DecC() {
  std::cout << "Decreasing C" << std::endl;
  uint8_t c = registers.getC();

  if (registers.getC() <= 0x10) {
    registers.setFlag(registers.FLAG_H, true);
  } else {
    registers.setFlag(registers.FLAG_H, false);
  }

  registers.setC(--c);

  registers.setFlag(registers.FLAG_Z, registers.getC() == 0);
  registers.setFlag(registers.FLAG_N, true);

  // throw std::runtime_error("stop from decC");
}

void CPU::LDCu8() { registers.setC(memory.read(registers.PC + 1)); }

void CPU::JRNZi8() {
  // std::cout << std::hex << (int)registers.PC << std::endl;
  int8_t offset = static_cast<int8_t>(memory.read(registers.PC + 1));

  if (!registers.isFlagSet(registers.FLAG_Z)) {
    registers.PC += offset;
  }
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
  std::cout << "HL address: " << std::hex
            << (int)toLittleEndian(highByte, lowByte) << std::endl;
  registers.HL = toLittleEndian(highByte, lowByte);
}

void CPU::LDHLDecA() {
  auto a = registers.getA();
  auto address = registers.HL;

  memory.write(address, a);
  registers.HL--;
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
  uint8_t highByte = memory.read(registers.PC + 1);
  uint8_t lowByte = memory.read(registers.PC + 2);
  // uint8_t highByte = memory.getMemoryBank(registers.PC)[registers.PC + 1];
  // uint8_t lowByte = memory.getMemoryBank(registers.PC)[registers.PC + 2];

  return {highByte, lowByte};
}
