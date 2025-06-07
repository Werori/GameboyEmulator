#include "Cpu.h"
#include "GameboyMemory.h"

#include <array>
#include <cstdint>
#include <functional>
#include <ios>
#include <iostream>

CPU::CPU(GameboyMemory &memory, PPU &ppu, Timers &timers)
    : memory(memory), ppu(ppu), timers(timers) {
  initializeOpcodes();
};

void CPU::initializeOpcodes() {
  opcodes[0x00] = {nop, 1, 4, 4};
  opcodes[0x01] = {[this]() { CPU::LDBCu16(); }, 3, 12, 12};
  opcodes[0x03] = {[this]() { CPU::IncBC(); }, 1, 8, 8};
  opcodes[0x05] = {[this]() { CPU::DecB(); }, 1, 4, 4};
  opcodes[0x06] = {[this]() { CPU::LDBu8(); }, 2, 8, 8};
  opcodes[0x0b] = {[this]() { CPU::DecBC(); }, 1, 8, 8};
  opcodes[0x0c] = {[this]() { CPU::IncC(); }, 1, 4, 4};
  opcodes[0x0d] = {[this]() { CPU::DecC(); }, 1, 4, 4};
  opcodes[0x0e] = {[this]() { CPU::LDCu8(); }, 2, 8, 8};
  // opcodes[0x11] = {[this]() { CPU::LDDEu16(); }, 3, 12, 12};
  // opcodes[0x12] = {[this]() { CPU::LDDEA(); }, 1, 8, 8};
  // opcodes[0x1c] = {[this]() { CPU::IncE(); }, 1, 4, 4};
  opcodes[0x20] = {[this]() { CPU::JRNZi8(); }, 2, 8, 12};
  opcodes[0x21] = {[this]() { CPU::LDHLu16(); }, 3, 12, 12};
  opcodes[0x28] = {[this]() { CPU::JRZi8(); }, 2, 8, 12};
  opcodes[0x2a] = {[this]() { CPU::LDAHLInc(); }, 1, 8, 8};
  opcodes[0x31] = {[this]() { CPU::LDSPu16(); }, 3, 12, 12};
  opcodes[0x32] = {[this]() { CPU::LDHLDecA(); }, 1, 8, 8};
  opcodes[0x36] = {[this]() { CPU::LDHLu8(); }, 2, 12, 12};
  opcodes[0x3e] = {[this]() { CPU::LDAu8(); }, 2, 8, 8};
  // opcodes[0x47] = {[this]() { CPU::LDBA(); }, 1, 4, 4};
  opcodes[0xaf] = {[this]() { CPU::XORAA(); }, 1, 4, 4};
  opcodes[0xb1] = {[this]() { CPU::ORAC(); }, 1, 4, 4};
  // opcodes[0xc0] = {[this]() { CPU::RetNz(); }, 1, 8, 20};
  opcodes[0xc3] = {[this]() { CPU::JPa16(); }, 3, 16, 16};
  opcodes[0xc5] = {[this]() { CPU::PushBC(); }, 1, 16, 16};
  opcodes[0xc9] = {[this]() { CPU::RET(); }, 1, 16, 16};
  opcodes[0xcd] = {[this]() { CPU::Call(); }, 3, 24, 24};
  opcodes[0xd5] = {[this]() { CPU::PushDE(); }, 1, 16, 16};
  // opcodes[0xcf] = {[this]() { CPU::RST08h(); }, 1, 16, 16};
  opcodes[0xa7] = {[this]() { CPU::AndAA(); }, 1, 4, 4};
  opcodes[0xe0] = {[this]() { CPU::LDFF00Plusu8A(); }, 2, 12, 12};
  opcodes[0xe2] = {[this]() { CPU::LDFF00PlusCA(); }, 1, 8, 8};
  opcodes[0xe5] = {[this]() { CPU::PushHL(); }, 1, 16, 16};
  opcodes[0xea] = {[this]() { CPU::LDu16A(); }, 3, 16, 16};
  opcodes[0xf0] = {[this]() { CPU::LDAFF00Plusu8(); }, 2, 12, 12};
  opcodes[0xf3] = {[this]() { CPU::DI(); }, 1, 4, 4};
  // opcodes[0xf9] = {[this]() { CPU::LDSPHL(); }, 1, 8, 8};
  opcodes[0xf5] = {[this]() { CPU::PushAF(); }, 1, 16, 16};
  opcodes[0x78] = {[this]() { CPU::LDAB(); }, 1, 4, 4};
  opcodes[0x7b] = {[this]() { CPU::LDAE(); }, 1, 4, 4};
  opcodes[0xfe] = {[this]() { CPU::CPAu8(); }, 2, 8, 8};
  opcodes[0x7f] = {[this]() { CPU::LDAA(); }, 1, 4, 4};
  opcodes[0xfb] = {[this]() { CPU::EI(); }, 1, 4, 4};
};

void CPU::fetchOpcode() {
  opcode = memory.read(registers.PC);
  // std::cout << "register A: " << (int)registers.getA() << std::endl;
  // std::cout << "register B: " << (int)registers.getB() << std::endl;
  // std::cout << "register C: " << (int)registers.getC() << std::endl;
  // std::cout << "register F: " << (int)registers.getF() << std::endl;
  // std::cout << "HL: 0x" << std::hex << registers.HL << std::endl;
}

void CPU::decodeAndExecute() {
  // std::cout << std::hex << "pc: " << (int)registers.PC << std::endl;
  // std::cout << std::hex << "pc: " << (int)registers.PC << std::endl;
  // std::cout << std::hex << "pc + 1: " << (int)memory.read(registers.PC +
  // 1)
  //           << std::endl;

  if (opcodes[opcode].func) {
    //
  } else {
    std::cout << std::hex << "opcode: " << (int)opcode << std::endl;
  }

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
  timers.tick(lastInstructionCycles);

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

void CPU::LDBCu16() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  registers.BC = toLittleEndian(lowByte, highByte);
}

void CPU::IncBC() { registers.BC = registers.BC++; }

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

void CPU::DecBC() { registers.BC--; }

void CPU::IncC() {
  uint8_t c = registers.getC();
  uint8_t result = c + 1;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, isHalfCarryAdd(c, 1));

  registers.setC(result);
}

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

  registers.setE(result);
}

void CPU::JRZi8() {
  int8_t offset = static_cast<int8_t>(memory.read(registers.PC + 1));
  if (registers.isFlagSet(registers.FLAG_Z)) {
    registers.PC = registers.PC + opcodes[opcode].length + offset;
    jumped = true;
  }
}

void CPU::JRNZi8() {
  int8_t offset = static_cast<int8_t>(memory.read(registers.PC + 1));
  // std::cout << "offset: " << std::dec << static_cast<int>(offset) <<
  // std::endl;

  if (!registers.isFlagSet(registers.FLAG_Z)) {
    registers.PC = registers.PC + opcodes[opcode].length + offset;
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

void CPU::ORAC() {
  registers.setA((registers.getA() | registers.getC()));

  registers.setFlag(registers.FLAG_Z, registers.getA() == 0);
  registers.setFlag(registers.FLAG_C, false);
  registers.setFlag(registers.FLAG_H, false);
  registers.setFlag(registers.FLAG_N, false);
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
  auto [lowByte, highByte] = getLowByteAndHighByte();
  registers.PC = toLittleEndian(lowByte, highByte);
  jumped = true;
};

void CPU::PushBC() { pushToStack(registers.BC); }

void CPU::RET() {
  uint8_t low = memory.read(registers.SP);
  registers.SP += 1;

  uint8_t high = memory.read(registers.SP);
  registers.SP += 1;

  registers.PC = toLittleEndian(low, high);
}

void CPU::Call() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  uint16_t address = toLittleEndian(lowByte, highByte);

  pushToStack(registers.PC + 3);

  registers.PC = address;
}

void CPU::PushDE() { pushToStack(registers.DE); }

void CPU::AndAA() {
  uint8_t a = registers.getA();
  registers.setA(a & a);

  registers.setFlag(registers.FLAG_Z, registers.getA() == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, true);
  registers.setFlag(registers.FLAG_C, false);
}

void CPU::RST08h() {
  // std::cout << "RST -----------------------------------" << std::endl;
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
  // std::cout << "SP: " << registers.SP << std::endl;
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

void CPU::LDAu8() { registers.setA(memory.read(registers.PC + 1)); }

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

void CPU::LDFF00PlusCA() {
  uint8_t a = registers.getA();
  uint8_t c = registers.getC();
  uint16_t address = 0xff00 + c;

  memory.write(address, a);
}

void CPU::PushHL() { pushToStack(registers.HL); }

void CPU::LDu16A() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  uint16_t address = toLittleEndian(lowByte, highByte);

  uint8_t value = registers.getA();
  memory.write(address, value);
  // auto add = memory.read(address);
  // std::cout << "value: " << std::hex << (int)value << std::endl;
  // std::cout << "add: " << std::hex << (int)add << std::endl;
  // throw std::runtime_error("reading....");
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

void CPU::PushAF() { pushToStack(registers.AF); }

void CPU::LDAB() { registers.setA(registers.getB()); }

void CPU::LDAE() { registers.setA(registers.getE()); }

void CPU::LDSPHL() { registers.SP = registers.HL; }

void CPU::CPAu8() {
  uint8_t value = memory.read(registers.PC + 1);
  uint8_t a = registers.getA();

  registers.setFlag(registers.FLAG_Z, a == value);
  registers.setFlag(registers.FLAG_N, true);
  registers.setFlag(registers.FLAG_H, isHalfCarrySub(a, value));
  registers.setFlag(registers.FLAG_C, isCarrySub(a, value));
}

void CPU::LDAA() {
  // do nothing
}

void CPU::EI() { IME = 1; }

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
    if (firedInterrupts & (1 << i)) {
      IME = 0; // disable interrupts

      // clear the interrupt request flag bit
      interruptFlag &= ~(1 << i);
      memory.write(0xFF0F, interruptFlag);

      // push current PC onto stack (16-bit)
      pushToStack(registers.PC);

      registers.PC = interruptVectors[i]; // jump to interrupt handler

      break; // handle one interrupt per call
    }
  }
}

void CPU::pushToStack(uint16_t address) {
  registers.SP--;
  memory.write(registers.SP, (address >> 8) & 0xff);
  registers.SP--;
  memory.write(registers.SP, (address & 0xff));
}
