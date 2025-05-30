#include "Cpu.h"
#include "GameboyMemory.h"

#include <array>
#include <cstdint>
#include <functional>
#include <ios>
#include <iostream>
#include <stdexcept>

CPU::CPU(GameboyMemory &memory, PPU &ppu) : memory(memory), ppu(ppu) {
  initializeOpcodes();
};

void CPU::initializeOpcodes() {
  opcodes[0x00] = {nop, 1, 4, 4};
  opcodes[0x05] = {[this]() { CPU::DecB(); }, 1, 4, 4};
  opcodes[0x06] = {[this]() { CPU::LDBu8(); }, 2, 8, 8};
  opcodes[0x0d] = {[this]() { CPU::DecC(); }, 1, 4, 4};
  opcodes[0x0e] = {[this]() { CPU::LDCu8(); }, 2, 8, 8};
  opcodes[0x20] = {[this]() { CPU::JRNZi8(); }, 2, 8, 12};
  opcodes[0x21] = {[this]() { CPU::LDHLu16(); }, 3, 12, 12};
  opcodes[0x32] = {[this]() { CPU::LDHLDecA(); }, 1, 8, 8};
  opcodes[0x3e] = {[this]() { CPU::LDAu8(); }, 2, 8, 8};
  opcodes[0xaf] = {[this]() { CPU::XORAA(); }, 1, 4, 4};
  opcodes[0xc3] = {[this]() { CPU::JPa16(); }, 3, 16, 16};
  opcodes[0xe0] = {[this]() { CPU::LDFF00Plusu8A(); }, 2, 12, 12};
  opcodes[0xf0] = {[this]() { CPU::LDAFF00Plusu8(); }, 2, 12, 12};
  opcodes[0xf3] = {[this]() { CPU::DI(); }, 1, 4, 4};
  opcodes[0xfe] = {[this]() { CPU::CPAu8(); }, 2, 8, 8};
};

void CPU::fetchOpcode() {
  std::cout << "READING MEMORY..." << std::endl;
  opcode = memory.read(registers.PC);
  std::cout << std::hex << "pc: " << (int)registers.PC << std::endl;
  // std::cout << "register A: " << (int)registers.getA() << std::endl;
  // std::cout << "register B: " << (int)registers.getB() << std::endl;
  // std::cout << "register C: " << (int)registers.getC() << std::endl;
  // std::cout << "register F: " << (int)registers.getF() << std::endl;
}
void CPU::decodeAndExecute() {
  std::cout << std::hex << "opcode: " << (int)opcode << std::endl;
  std::cout << std::hex << "pc + 1: " << (int)memory.read(registers.PC + 1)
            << std::endl;
  if (opcode != 0xcb) {
    opcodes[opcode].func();
    length = opcodes[opcode].length;
  } else {
    std::cout << "Extended opcode!" << std::endl;
    registers.PC += 1;
    opcode = memory.read(registers.PC);
    extendedOpcodes[opcode].func();
    length = extendedOpcodes[opcode].length;
  }

  if (jumped) {
    cycles += opcodes[opcode].machine_cycles_branch;
    jumped = false;
  } else {
    cycles += opcodes[opcode].machine_cycles_no_branch;
    registers.PC += length;
  }

  ppu.tick(cycles);
}

void CPU::nop() { std::cout << "Nop function" << std::endl; };

void CPU::DecB() {
  std::cout << "B before: " << (int)registers.getB();
  uint8_t b = registers.getB();

  registers.setFlag(registers.FLAG_H, (b & LOWER_NIBBLE_MASK) == 0);

  b -= 1;
  registers.setB(b);

  registers.setFlag(registers.FLAG_Z, registers.getB() == 0);
  registers.setFlag(registers.FLAG_N, true);

  std::cout << std::hex << "FLAG H: " << (int)registers.FLAG_H << std::endl;
  std::cout << "B after: " << (int)registers.getB();
}

void CPU::LDBu8() { registers.setB(memory.read(registers.PC + 1)); }

void CPU::DecC() {
  uint8_t c = registers.getC();

  registers.setFlag(registers.FLAG_H, (c & LOWER_NIBBLE_MASK) == 0);

  c -= 1;
  registers.setC(c);

  registers.setFlag(registers.FLAG_Z, registers.getC() == 0);
  registers.setFlag(registers.FLAG_N, true);
}

void CPU::LDCu8() { registers.setC(memory.read(registers.PC + 1)); }

void CPU::JRNZi8() {
  int8_t offset = static_cast<int8_t>(memory.read(registers.PC + 1));
  registers.PC += opcodes[opcode].length;

  if (!registers.isFlagSet(registers.FLAG_Z)) {
    registers.PC += offset;
    jumped = true;
  }
}

void CPU::XORAA() {
  uint8_t result = registers.getA() ^ registers.getA();

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, false);
  registers.setFlag(registers.FLAG_C, false);
}

void CPU::JPa16() {

  // std::cout << "pc: " << std::hex << registers.PC << std::endl;
  auto [lowByte, highByte] = getLowByteAndHighByte();

  // std::cout << "high: " << std::hex << (int)highByte << std::endl;

  // std::cout << "low:" << std::hex << (int)lowByte << std::endl;

  registers.PC = CPU::toLittleEndian(lowByte, highByte);
  jumped = true;
};

void CPU::LDHLu16() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  std::cout << "HL address: " << std::hex
            << (int)toLittleEndian(lowByte, highByte) << std::endl;
  registers.HL = toLittleEndian(lowByte, highByte);
}

void CPU::LDHLDecA() {
  auto a = registers.getA();
  auto address = registers.HL;

  memory.write(address, a);
  registers.HL--;
}

void CPU::LDAu8() { registers.setC(memory.read(registers.PC + 1)); }

void CPU::LDFF00Plusu8A() {
  auto a = registers.getA();
  auto byte = memory.read(registers.PC + 1);
  // auto byte = registers.PC + 1;
  // std::cout << std::hex << "byte: " << (int)byte << std::endl;
  uint16_t address = 0xff00 + byte;

  // std::cout << "io address: " << std::hex << (int)address << std::endl;

  memory.write(address, a);
}

void CPU::LDAFF00Plusu8() {
  auto byte = memory.read(registers.PC + 1);
  std::cout << std::hex << "byte: " << (int)byte << std::endl;
  auto address = memory.read(0xff00 + byte);
  // std::cout << std::hex << "address: " << address << std::endl;

  registers.setA(address);
}

void CPU::DI() { IME = 0; }

void CPU::CPAu8() {
  uint8_t value = memory.read(registers.PC + 1);
  uint8_t a = registers.getA();

  std::cout << "z: " << (a == value) << " a: " << (int)a
            << "value: " << (int)value << std::endl;

  registers.setFlag(registers.FLAG_Z, a == value);
  registers.setFlag(registers.FLAG_N, true);
  registers.setFlag(registers.FLAG_H, isHalfCarry(a, value));
  registers.setFlag(registers.FLAG_C, isCarry(a, value));
}

uint16_t CPU::toLittleEndian(uint8_t lowByte, uint8_t highByte) {
  return (highByte << 8) | lowByte;
}

std::pair<uint8_t, uint8_t> CPU::getLowByteAndHighByte() {
  uint8_t lowByte = memory.read(registers.PC + 1);
  uint8_t highByte = memory.read(registers.PC + 2);
  return {lowByte, highByte};
}

bool CPU::isHalfCarry(uint8_t a, uint8_t value) {
  return ((a & 0x0f) < (value & 0x0f));
}

bool CPU::isCarry(uint8_t a, uint8_t value) {
  return (a < (value + (registers.getF() & CARRY_FLAG_REGISTER_F)));
}
