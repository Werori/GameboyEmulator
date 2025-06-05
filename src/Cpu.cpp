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
  // opcodes[0x05] = {[this]() { CPU::DecB(); }, 1, 4, 4};
  // opcodes[0x06] = {[this]() { CPU::LDBu8(); }, 2, 8, 8};
  // opcodes[0x0d] = {[this]() { CPU::DecC(); }, 1, 4, 4};
  // opcodes[0x0e] = {[this]() { CPU::LDCu8(); }, 2, 8, 8};
  // opcodes[0x11] = {[this]() { CPU::LDDEu16(); }, 3, 12, 12};
  // opcodes[0x12] = {[this]() { CPU::LDDEA(); }, 1, 8, 8};
  // opcodes[0x1c] = {[this]() { CPU::IncE(); }, 1, 4, 4};
  // opcodes[0x20] = {[this]() { CPU::JRNZi8(); }, 2, 8, 12};
  // opcodes[0x21] = {[this]() { CPU::LDHLu16(); }, 3, 12, 12};
  // opcodes[0x2a] = {[this]() { CPU::LDAHLInc(); }, 1, 8, 8};
  // opcodes[0x31] = {[this]() { CPU::LDSPu16(); }, 3, 12, 12};
  // opcodes[0x32] = {[this]() { CPU::LDHLDecA(); }, 1, 8, 8};
  // opcodes[0x36] = {[this]() { CPU::LDHLu8(); }, 2, 12, 12};
  // opcodes[0x3e] = {[this]() { CPU::LDAu8(); }, 2, 8, 8};
  // opcodes[0x47] = {[this]() { CPU::LDBA(); }, 1, 4, 4};
  // opcodes[0xaf] = {[this]() { CPU::XORAA(); }, 1, 4, 4};
  // opcodes[0xc0] = {[this]() { CPU::RetNz(); }, 1, 8, 20};
  opcodes[0xc3] = {[this]() { CPU::JPa16(); }, 3, 16, 16};
  // opcodes[0xcf] = {[this]() { CPU::RST08h(); }, 1, 16, 16};
  // opcodes[0xe0] = {[this]() { CPU::LDFF00Plusu8A(); }, 2, 12, 12};
  // opcodes[0xea] = {[this]() { CPU::LDu16A(); }, 3, 16, 16};
  // opcodes[0xf0] = {[this]() { CPU::LDAFF00Plusu8(); }, 2, 12, 12};
  opcodes[0xf3] = {[this]() { CPU::DI(); }, 1, 4, 4};
  // opcodes[0xf9] = {[this]() { CPU::LDSPHL(); }, 1, 8, 8};
  // opcodes[0xfe] = {[this]() { CPU::CPAu8(); }, 2, 8, 8};
};

void CPU::fetchOpcode() {
  opcode = memory.read(registers.PC);
  std::cout << std::hex << "pc: " << (int)registers.PC << std::endl;
  // std::cout << "register A: " << (int)registers.getA() << std::endl;
  // std::cout << "register B: " << (int)registers.getB() << std::endl;
  // std::cout << "register C: " << (int)registers.getC() << std::endl;
  // std::cout << "register F: " << (int)registers.getF() << std::endl;
}
void CPU::decodeAndExecute() {
  std::cout << std::hex << "opcode: " << (int)opcode << std::endl;
  // std::cout << std::hex << "pc: " << (int)registers.PC << std::endl;
  // std::cout << std::hex << "pc + 1: " << (int)memory.read(registers.PC +
  // 1)
  //           << std::endl;

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
    // cycles += opcodes[opcode].machine_cycles_branch;
    lastInstructionCycles = opcodes[opcode].machine_cycles_branch;
    jumped = false;
  } else {
    // cycles += opcodes[opcode].machine_cycles_no_branch;
    lastInstructionCycles = opcodes[opcode].machine_cycles_no_branch;
    registers.PC += length;
  }

  ppu.tick(lastInstructionCycles);
  memory.dividerCounter += lastInstructionCycles;

  while (memory.dividerCounter >= 256) {
    memory.increaseDivider();
    memory.dividerCounter -= 256;
  }

  cycles += lastInstructionCycles;
  handleInterrupts();
}

void CPU::nop() {
  // std::cout << "Nop function" << std::endl;

};

void CPU::DecB() {
  // std::cout << "B before: " << (int)registers.getB();
  uint8_t b = registers.getB();
  uint8_t result = b - 1;

  registers.setFlag(registers.FLAG_H, isHalfCarrySub(b, 1));

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, true);

  registers.setB(result);
  // std::cout << std::hex << "FLAG H: " << (int)registers.FLAG_H << std::endl;
  // std::cout << "B after: " << (int)registers.getB();
}

void CPU::LDBu8() { registers.setB(memory.read(registers.PC + 1)); }

void CPU::DecC() {
  uint8_t c = registers.getC();
  uint8_t result = c - 1;

  registers.setFlag(registers.FLAG_H, isHalfCarrySub(c, 1));

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, true);

  registers.setC(result);
}

void CPU::LDCu8() { registers.setC(memory.read(registers.PC + 1)); }

void CPU::LDDEu16() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  uint16_t address = toLittleEndian(lowByte, highByte);
  registers.DE = address;
}

void CPU::LDDEA() {
  auto a = registers.getA();
  auto address = registers.DE;
  memory.write(address, a);
}

void CPU::IncE() {
  uint8_t e = registers.getE();
  uint8_t result = e + 1;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, isHalfCarryAdd(e, 1));

  std::cout << "INC E: before=" << +e << " after=" << +result
            << " Z=" << registers.isFlagSet(registers.FLAG_Z)
            << " H=" << registers.isFlagSet(registers.FLAG_H) << std::endl;
  registers.setE(result);
}

void CPU::JRNZi8() {
  int8_t offset = static_cast<int8_t>(memory.read(registers.PC + 1));

  if (!registers.isFlagSet(registers.FLAG_Z)) {
    registers.PC = opcodes[opcode].length + offset;
    jumped = true;
  }
}

void CPU::XORAA() {
  uint8_t result = registers.getA() ^ registers.getA();

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, false);
  registers.setFlag(registers.FLAG_C, false);

  registers.setA(result);
}

void CPU::RetNz() {
  if (!registers.isFlagSet(registers.FLAG_Z)) {
    // Pop return address from stack
    uint8_t low = memory.read(registers.SP);
    uint8_t high = memory.read(registers.SP + 1);
    registers.SP += 2;

    uint16_t returnAddress = (high << 8) | low;
    registers.PC = returnAddress;

    jumped = true; // Indicate we changed PC by a jump/return
  } else {
    // Do nothing, just continue; PC will move past the opcode naturally
    jumped = false;
  }
}

void CPU::JPa16() {

  // std::cout << "pc in jump before: " << std::hex << registers.PC <<
  // std::endl;
  auto [lowByte, highByte] = getLowByteAndHighByte();

  // std::cout << "high: " << std::hex << (int)highByte << std::endl;

  // std::cout << "low:" << std::hex << (int)lowByte << std::endl;

  registers.PC = CPU::toLittleEndian(lowByte, highByte);
  // std::cout << "pc in jump is: " << (int)registers.PC << std::endl;
  jumped = true;
};

void CPU::RST08h() {
  std::cout << "RST -----------------------------------" << std::endl;
  uint16_t pc = registers.PC + 1;

  registers.SP -= 1;
  memory.write(registers.SP, pc & 0xff);

  registers.SP -= 1;
  memory.write(registers.SP, (pc >> 8) & 0xff);

  registers.PC = 0x08;
  jumped = true;
}

void CPU::LDHLu16() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  // std::cout << "HL address: " << std::hex
  // << (int)toLittleEndian(lowByte, highByte) << std::endl;
  registers.HL = toLittleEndian(lowByte, highByte);
}

void CPU::LDAHLInc() {
  auto value = memory.read(registers.HL);
  registers.setA(value);
  registers.HL = registers.HL + 1;
}

void CPU::LDSPu16() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  uint16_t address = toLittleEndian(lowByte, highByte);
  registers.SP = address;
}

void CPU::LDHLDecA() {
  auto a = registers.getA();
  auto address = registers.HL;

  memory.write(address, a);
  // std::cout << "HL: " << std::hex << (int)address << std::endl;
  registers.HL--;
}

void CPU::LDHLu8() {
  uint8_t value = memory.read(registers.PC + 1);
  memory.write(registers.HL, value);
}

void CPU::LDAu8() { registers.setC(memory.read(registers.PC + 1)); }

void CPU::LDBA() {
  uint8_t a = registers.getA();
  registers.setB(a);
}

void CPU::LDFF00Plusu8A() {
  auto a = registers.getA();
  auto byte = memory.read(registers.PC + 1);
  // auto byte = registers.PC + 1;
  // std::cout << std::hex << "byte: " << (int)byte << std::endl;
  uint16_t address = 0xff00 + byte;

  // std::cout << "io address: " << std::hex << (int)address << std::endl;

  memory.write(address, a);
}

void CPU::LDu16A() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  uint16_t address = toLittleEndian(lowByte, highByte);

  uint8_t value = registers.getA();
  memory.write(address, value);
}

void CPU::LDAFF00Plusu8() {
  auto byte = memory.read(registers.PC + 1);
  // std::cout << std::hex << "byte: " << (int)byte << std::endl;
  // auto address = memory.read(0xff00 + byte);
  auto address = 0xff00 + byte;
  uint8_t value = memory.read(address);
  // std::cout << std::hex << "address: " << address << std::endl;

  registers.setA(value);
}

void CPU::DI() { IME = 0; }

void CPU::LDSPHL() { registers.SP = registers.HL; }

void CPU::CPAu8() {
  uint8_t value = memory.read(registers.PC + 1);
  uint8_t a = registers.getA();

  // std::cout << "CPA value: " << (int)value << std::endl;

  registers.setFlag(registers.FLAG_Z, a == value);
  // std::cout << "Compare A=" << (int)a << " to value=" << (int)value
  // << std::endl;
  // std::cout << "Ly is: " << (int)memory.getLy() << std::endl;
  registers.setFlag(registers.FLAG_N, true);
  registers.setFlag(registers.FLAG_H, isHalfCarrySub(a, value));
  registers.setFlag(registers.FLAG_C, isCarrySub(a, value));
}

uint16_t CPU::toLittleEndian(uint8_t lowByte, uint8_t highByte) {
  return (static_cast<uint16_t>(lowByte)) |
         (static_cast<uint16_t>(highByte) << 8);
}

std::pair<uint8_t, uint8_t> CPU::getLowByteAndHighByte() {
  uint8_t lowByte = memory.read(registers.PC + 1);
  uint8_t highByte = memory.read(registers.PC + 2);
  return {lowByte, highByte};
}

bool CPU::isHalfCarryAdd(uint8_t a, uint8_t value) {
  return ((a & 0x0f) + (value & 0x0f) > 0x0f);
}

bool CPU::isHalfCarrySub(uint8_t a, uint8_t value) {
  return (a & 0x0f) < (value & 0x0f);
}

bool CPU::isCarrySub(uint8_t a, uint8_t value) { return a < value; }

bool CPU::isCarryAdd(uint8_t a, uint8_t value) {
  return (uint16_t)a + (uint16_t)value > 0xFF;
}

void CPU::handleInterrupts() {
  if (!IME) {
    return;
  }
  uint8_t interruptFlag = memory.read(0xFF0F);
  uint8_t interruptEnable = memory.read(0xFFFF);

  uint8_t firedInterrupts = interruptFlag & interruptEnable;
  // throw std::runtime_error("interrupts off fired");

  if (firedInterrupts == 0) {
    return; // no interrupt requested or enabled
  }

  // Interrupts have priority from bit 0 to 4:
  const uint16_t interruptVectors[5] = {
      0x40, // V-Blank
      0x48, // LCD STAT
      0x50, // Timer
      0x58, // Serial
      0x60  // Joypad
  };

  for (int i = 0; i < 5; ++i) {
    throw std::runtime_error("HERE");
    if (firedInterrupts & (1 << i)) {
      IME = 0; // disable interrupts

      // clear the interrupt request flag bit
      interruptFlag &= ~(1 << i);
      memory.write(0xFF0F, interruptFlag);

      // push current PC onto stack (16-bit)
      registers.SP -= 1;
      memory.write(registers.SP, registers.PC & 0xFF); // low byte
      registers.SP -= 1;
      memory.write(registers.SP, (registers.PC >> 8) & 0xFF); // high byte

      registers.PC = interruptVectors[i]; // jump to interrupt handler

      break; // handle one interrupt per call
    }
  }
}
