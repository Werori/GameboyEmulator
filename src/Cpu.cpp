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
  initializeExtendedOpcodes();
};

void CPU::initializeOpcodes() {
  opcodes[0x00] = {nop, 1, 4, 4};
  opcodes[0x01] = {[this]() { CPU::LDBCu16(); }, 3, 12, 12};
  opcodes[0x02] = {[this]() { CPU::LDBCA(); }, 1, 8, 8};
  opcodes[0x03] = {[this]() { CPU::IncBC(); }, 1, 8, 8};
  opcodes[0x04] = {[this]() { CPU::IncB(); }, 1, 4, 4};
  opcodes[0x05] = {[this]() { CPU::DecB(); }, 1, 4, 4};
  opcodes[0x06] = {[this]() { CPU::LDBu8(); }, 2, 8, 8};
  opcodes[0x07] = {[this]() { CPU::RLCA(); }, 1, 4, 4};
  opcodes[0x0a] = {[this]() { CPU::LDABC(); }, 1, 8, 8};
  opcodes[0x0b] = {[this]() { CPU::DecBC(); }, 1, 8, 8};
  opcodes[0x0c] = {[this]() { CPU::IncC(); }, 1, 4, 4};
  opcodes[0x0d] = {[this]() { CPU::DecC(); }, 1, 4, 4};
  opcodes[0x0e] = {[this]() { CPU::LDCu8(); }, 2, 8, 8};
  opcodes[0x10] = {[this]() { CPU::STOP(); }, 2, 4, 4};
  opcodes[0x11] = {[this]() { CPU::LDDEu16(); }, 3, 12, 12};
  opcodes[0x12] = {[this]() { CPU::LDDEA(); }, 1, 8, 8};
  opcodes[0x13] = {[this]() { CPU::IncDE(); }, 1, 8, 8};
  opcodes[0x16] = {[this]() { CPU::LDDu8(); }, 2, 8, 8};
  opcodes[0x18] = {[this]() { CPU::JRi8(); }, 2, 12, 12};
  opcodes[0x19] = {[this]() { CPU::AddHLDE(); }, 1, 8, 8};
  opcodes[0x1a] = {[this]() { CPU::LDADE(); }, 1, 8, 8};
  opcodes[0x1c] = {[this]() { CPU::IncE(); }, 1, 4, 4};
  opcodes[0x1e] = {[this]() { CPU::LDEu8(); }, 2, 8, 8};
  opcodes[0x20] = {[this]() { CPU::JRNZi8(); }, 2, 8, 12};
  opcodes[0x21] = {[this]() { CPU::LDHLu16(); }, 3, 12, 12};
  opcodes[0x22] = {[this]() { CPU::LDHLPlusA(); }, 1, 8, 8};
  opcodes[0x23] = {[this]() { CPU::IncHL(); }, 1, 8, 8};
  opcodes[0x25] = {[this]() { CPU::DecH(); }, 1, 4, 4};
  opcodes[0x26] = {[this]() { CPU::LDHu8(); }, 2, 8, 8};
  opcodes[0x28] = {[this]() { CPU::JRZi8(); }, 2, 8, 12};
  opcodes[0x29] = {[this]() { CPU::AddHLHL(); }, 1, 8, 8};
  opcodes[0x2a] = {[this]() { CPU::LDAHLInc(); }, 1, 8, 8};
  opcodes[0x2b] = {[this]() { CPU::DecHL(); }, 1, 8, 8};
  opcodes[0x2c] = {[this]() { CPU::IncL(); }, 1, 4, 4};
  opcodes[0x2e] = {[this]() { CPU::LDLu8(); }, 2, 8, 8};
  opcodes[0x2f] = {[this]() { CPU::CPL(); }, 1, 4, 4};
  opcodes[0x31] = {[this]() { CPU::LDSPu16(); }, 3, 12, 12};
  opcodes[0x32] = {[this]() { CPU::LDHLDecA(); }, 1, 8, 8};
  opcodes[0x34] = {[this]() { CPU::IncHLAddr(); }, 1, 12, 12};
  opcodes[0x35] = {[this]() { CPU::DecAddrHL(); }, 1, 12, 12};
  opcodes[0x36] = {[this]() { CPU::LDHLu8(); }, 2, 12, 12};
  opcodes[0x38] = {[this]() { CPU::JRCi8(); }, 2, 8, 12};
  opcodes[0x3a] = {[this]() { CPU::LDAHLDec(); }, 1, 8, 8};
  opcodes[0x3b] = {[this]() { CPU::DecSP(); }, 1, 8, 8};
  opcodes[0x3c] = {[this]() { CPU::IncA(); }, 1, 4, 4};
  opcodes[0x3d] = {[this]() { CPU::DecA(); }, 1, 4, 4};
  opcodes[0x3e] = {[this]() { CPU::LDAu8(); }, 2, 8, 8};
  opcodes[0x42] = {[this]() { CPU::LDBD(); }, 1, 4, 4};
  opcodes[0x47] = {[this]() { CPU::LDBA(); }, 1, 4, 4};
  opcodes[0x4f] = {[this]() { CPU::LDCA(); }, 1, 4, 4};
  opcodes[0x56] = {[this]() { CPU::LDDHL(); }, 1, 8, 8};
  opcodes[0x57] = {[this]() { CPU::LDDA(); }, 1, 4, 4};
  opcodes[0x59] = {[this]() { CPU::LDEC(); }, 1, 4, 4};
  opcodes[0x5e] = {[this]() { CPU::LDEHL(); }, 1, 8, 8};
  opcodes[0x5f] = {[this]() { CPU::LDEA(); }, 1, 4, 4};
  opcodes[0x67] = {[this]() { CPU::LDHA(); }, 1, 4, 4};
  opcodes[0x6b] = {[this]() { CPU::LDLE(); }, 1, 4, 4};
  opcodes[0x6f] = {[this]() { CPU::LDLA(); }, 1, 4, 4};
  opcodes[0x77] = {[this]() { CPU::LDHLA(); }, 1, 8, 8};
  opcodes[0x78] = {[this]() { CPU::LDAB(); }, 1, 4, 4};
  opcodes[0x7a] = {[this]() { CPU::LDAD(); }, 1, 4, 4};
  opcodes[0x7b] = {[this]() { CPU::LDAE(); }, 1, 4, 4};
  opcodes[0x7c] = {[this]() { CPU::LDAH(); }, 1, 4, 4};
  opcodes[0x7d] = {[this]() { CPU::LDAL(); }, 1, 4, 4};
  opcodes[0x7e] = {[this]() { CPU::LDAHL(); }, 1, 8, 8};
  opcodes[0x7f] = {[this]() { CPU::LDAA(); }, 1, 4, 4};
  opcodes[0x80] = {[this]() { CPU::AddAB(); }, 1, 4, 4};
  opcodes[0x8f] = {[this]() { CPU::ADCAA(); }, 1, 4, 4};
  opcodes[0x89] = {[this]() { CPU::ADCAC(); }, 1, 4, 4};
  opcodes[0xa7] = {[this]() { CPU::AndAA(); }, 1, 4, 4};
  opcodes[0x90] = {[this]() { CPU::SubAB(); }, 1, 4, 4};
  opcodes[0x91] = {[this]() { CPU::SubAC(); }, 1, 4, 4};
  opcodes[0xa1] = {[this]() { CPU::AndAC(); }, 1, 4, 4};
  opcodes[0xa5] = {[this]() { CPU::ANDAL(); }, 1, 4, 4};
  opcodes[0xa9] = {[this]() { CPU::XORAC(); }, 1, 4, 4};
  opcodes[0xaf] = {[this]() { CPU::XORAA(); }, 1, 4, 4};
  opcodes[0xb0] = {[this]() { CPU::ORAB(); }, 1, 4, 4};
  opcodes[0xb1] = {[this]() { CPU::ORAC(); }, 1, 4, 4};
  opcodes[0xb2] = {[this]() { CPU::ORAD(); }, 1, 4, 4};
  opcodes[0xb9] = {[this]() { CPU::CPAC(); }, 1, 4, 4};
  opcodes[0xc0] = {[this]() { CPU::RetNz(); }, 1, 8, 20};
  opcodes[0xc1] = {[this]() { CPU::PopBC(); }, 1, 12, 12};
  opcodes[0xc3] = {[this]() { CPU::JPa16(); }, 3, 16, 16};
  opcodes[0xc5] = {[this]() { CPU::PushBC(); }, 1, 16, 16};
  opcodes[0xc8] = {[this]() { CPU::RetZ(); }, 1, 8, 20};
  opcodes[0xc9] = {[this]() { CPU::RET(); }, 1, 16, 16};
  opcodes[0xca] = {[this]() { CPU::JPZu16(); }, 3, 12, 16};
  opcodes[0xcc] = {[this]() { CPU::CallZu16(); }, 3, 12, 24};
  opcodes[0xcd] = {[this]() { CPU::Call(); }, 3, 24, 24};
  // opcodes[0xcf] = {[this]() { CPU::RST08h(); }, 1, 16, 16};
  opcodes[0xd1] = {[this]() { CPU::PopDE(); }, 1, 12, 12};
  opcodes[0xd5] = {[this]() { CPU::PushDE(); }, 1, 16, 16};
  opcodes[0xd6] = {[this]() { CPU::SubAu8(); }, 2, 8, 8};
  opcodes[0xd9] = {[this]() { CPU::Reti(); }, 1, 16, 16};
  opcodes[0xde] = {[this]() { CPU::SBCAu8(); }, 2, 8, 8};
  opcodes[0xe0] = {[this]() { CPU::LDFF00Plusu8A(); }, 2, 12, 12};
  opcodes[0xe1] = {[this]() { CPU::PopHL(); }, 1, 12, 12};
  opcodes[0xe2] = {[this]() { CPU::LDFF00PlusCA(); }, 1, 8, 8};
  opcodes[0xe5] = {[this]() { CPU::PushHL(); }, 1, 16, 16};
  opcodes[0xe6] = {[this]() { CPU::AndAu8(); }, 2, 8, 8};
  opcodes[0xe8] = {[this]() { CPU::AddSPi8(); }, 2, 16, 16};
  opcodes[0xea] = {[this]() { CPU::LDu16A(); }, 3, 16, 16};
  opcodes[0xee] = {[this]() { CPU::XORAu8(); }, 2, 8, 8};
  opcodes[0xf0] = {[this]() { CPU::LDAFF00Plusu8(); }, 2, 12, 12};
  opcodes[0xf1] = {[this]() { CPU::PopAF(); }, 1, 12, 12};
  opcodes[0xf3] = {[this]() { CPU::DI(); }, 1, 4, 4};
  // opcodes[0xf9] = {[this]() { CPU::LDSPHL(); }, 1, 8, 8};
  opcodes[0xf5] = {[this]() { CPU::PushAF(); }, 1, 16, 16};
  opcodes[0xfa] = {[this]() { CPU::LDAu16(); }, 3, 16, 16};
  opcodes[0xfe] = {[this]() { CPU::CPAu8(); }, 2, 8, 8};
  opcodes[0xfb] = {[this]() { CPU::EI(); }, 1, 4, 4};
};

void CPU::initializeExtendedOpcodes() {
  extendedOpcodes[0x37] = {[this]() { CPU::SwapA(); }, 2, 8, 8};
  extendedOpcodes[0x47] = {[this]() { CPU::Bit0A(); }, 2, 8, 8};
  extendedOpcodes[0x6f] = {[this]() { CPU::Bit5A(); }, 2, 8, 8};
  extendedOpcodes[0x77] = {[this]() { CPU::Bit6A(); }, 2, 8, 8};
}

void CPU::fetchOpcode() { opcode = memory.read(registers.PC); }

void CPU::decodeAndExecute() {
  if (opcode != 0xcb) {
    if (!opcodes[opcode].func) {
      std::cout << std::hex << "opcode: " << (int)opcode << std::endl;
    }
  } else {
    if (!extendedOpcodes[memory.read(registers.PC + 1)].func) {
      std::cout << std::hex
                << "extended opcode: " << (int)memory.read(registers.PC + 1)
                << std::endl;
    }
  }

  Instruction *instr = nullptr;

  if (opcode != 0xcb) {
    instr = &opcodes[opcode];
    instr->func();
  } else {
    registers.PC += 1;
    opcode = memory.read(registers.PC);
    instr = &extendedOpcodes[opcode];
    instr->func();
  }

  length = instr->length;

  if (jumped) {
    lastInstructionCycles = instr->machine_cycles_branch;
    jumped = false;
  } else {
    lastInstructionCycles = instr->machine_cycles_no_branch;
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

};

void CPU::LDBCu16() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  registers.BC = toLittleEndian(lowByte, highByte);
}

void CPU::LDBCA() {
  uint8_t a = registers.getA();
  memory.write(registers.BC, a);
}

void CPU::IncBC() { registers.BC = registers.BC++; }

void CPU::IncB() {
  uint8_t b = registers.getB();
  uint8_t result = b + 1;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, isHalfCarryAdd(b, 1));

  registers.setB(result);
}

void CPU::DecB() {
  uint8_t b = registers.getB();
  uint8_t result = b - 1;

  registers.setFlag(registers.FLAG_H, isHalfCarrySub(b, 1));

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, true);

  registers.setB(result);
}

void CPU::LDBu8() { registers.setB(memory.read(registers.PC + 1)); }

void CPU::RLCA() {
  uint8_t oldA = registers.getA();
  uint8_t bit7 = (oldA >> 7) & 0x1;
  registers.setA((oldA << 1) | bit7);

  registers.setFlag(registers.FLAG_Z, false);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, false);
  registers.setFlag(registers.FLAG_C, bit7 != 0);
}

void CPU::LDABC() { registers.setA(memory.read(registers.BC)); }

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

void CPU::STOP() {}

void CPU::LDDEu16() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  uint16_t address = toLittleEndian(lowByte, highByte);
  registers.DE = address;
}

void CPU::JRi8() {
  int8_t offset = static_cast<int8_t>(memory.read(registers.PC + 1));
  registers.PC = registers.PC + opcodes[opcode].length + offset;
  jumped = true;
}

void CPU::AddHLDE() {
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H,
                    isHalfCarryAdd16(registers.HL, registers.DE));
  registers.setFlag(registers.FLAG_C, isCarryAdd16(registers.HL, registers.DE));

  registers.HL += registers.DE;
}

void CPU::LDADE() {
  auto value = memory.read(registers.DE);
  registers.setA(value);
}

void CPU::LDDEA() {
  auto a = registers.getA();
  auto address = registers.DE;
  memory.write(address, a);
}

void CPU::IncDE() { registers.DE = registers.DE + 1; }

void CPU::LDDu8() {
  uint8_t value = memory.read(registers.PC + 1);
  registers.setD(value);
}

void CPU::IncE() {
  uint8_t e = registers.getE();
  uint8_t result = e + 1;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, isHalfCarryAdd(e, 1));

  registers.setE(result);
}

void CPU::LDEu8() {
  uint8_t value = memory.read(registers.PC + 1);
  registers.setE(value);
}

void CPU::JRZi8() {
  int8_t offset = static_cast<int8_t>(memory.read(registers.PC + 1));
  if (registers.isFlagSet(registers.FLAG_Z)) {
    registers.PC = registers.PC + opcodes[opcode].length + offset;
    jumped = true;
  }
}

void CPU::DecH() {
  uint8_t h = registers.getH();
  uint8_t result = h - 1;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, true);
  registers.setFlag(registers.FLAG_H, isHalfCarrySub(h, 1));
}

void CPU::LDHu8() {
  uint8_t value = memory.read(registers.PC + 1);
  registers.setH(value);
}

void CPU::JRNZi8() {
  int8_t offset = static_cast<int8_t>(memory.read(registers.PC + 1));

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

void CPU::ORAB() {
  registers.setA((registers.getA() | registers.getB()));

  registers.setFlag(registers.FLAG_Z, registers.getA() == 0);
  registers.setFlag(registers.FLAG_C, false);
  registers.setFlag(registers.FLAG_H, false);
  registers.setFlag(registers.FLAG_N, false);
}

void CPU::ORAC() {
  registers.setA((registers.getA() | registers.getC()));

  registers.setFlag(registers.FLAG_Z, registers.getA() == 0);
  registers.setFlag(registers.FLAG_C, false);
  registers.setFlag(registers.FLAG_H, false);
  registers.setFlag(registers.FLAG_N, false);
}

void CPU::ORAD() {
  uint8_t a = registers.getA();
  uint8_t d = registers.getD();
  uint8_t result = a | d;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_C, false);
  registers.setFlag(registers.FLAG_H, false);
  registers.setFlag(registers.FLAG_N, false);

  registers.setA(result);
}

void CPU::CPAC() {
  uint8_t a = registers.getA();
  uint8_t c = registers.getC();
  uint8_t result = a - c;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, true);
  registers.setFlag(registers.FLAG_H, isHalfCarrySub(a, c));
  registers.setFlag(registers.FLAG_C, isCarrySub(a, c));
}

void CPU::RetNz() {
  if (!registers.isFlagSet(registers.FLAG_Z)) {
    registers.PC = popFromStack();
    jumped = true;
  } else {
    jumped = false;
  }
}

void CPU::PopBC() { registers.BC = popFromStack(); }

void CPU::JPa16() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  registers.PC = toLittleEndian(lowByte, highByte);
  jumped = true;
};

void CPU::PushBC() { pushToStack(registers.BC); }

void CPU::RetZ() {
  if (registers.isFlagSet(registers.FLAG_Z) == 1) {
    registers.PC = popFromStack();
    jumped = true;
  }
}

void CPU::RET() {
  registers.PC = popFromStack();
  jumped = true;
}

void CPU::JPZu16() {
  if (registers.isFlagSet(registers.FLAG_Z)) {
    auto [lowByte, highByte] = getLowByteAndHighByte();
    registers.PC = toLittleEndian(lowByte, highByte);
    jumped = true;
  }
}

void CPU::CallZu16() {
  if (registers.isFlagSet(registers.FLAG_Z)) {
    auto [lowByte, highByte] = getLowByteAndHighByte();
    uint16_t address = toLittleEndian(lowByte, highByte);
    pushToStack(registers.PC + 3);

    registers.PC = address;
    jumped = true;
  }
}

void CPU::Call() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  uint16_t address = toLittleEndian(lowByte, highByte);

  pushToStack(registers.PC + 3);

  registers.PC = address;
  jumped = true;
}

void CPU::PushDE() { pushToStack(registers.DE); }

void CPU::SubAu8() {
  uint8_t value = memory.read(registers.PC + 1);
  uint8_t a = registers.getA();
  uint8_t result = a - value;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, true);
  registers.setFlag(registers.FLAG_H, isHalfCarrySub(a, value));
  registers.setFlag(registers.FLAG_C, isCarrySub(a, value));

  registers.setA(result);
}

void CPU::Reti() {
  registers.PC = popFromStack();
  IME = 1;
}

void CPU::SBCAu8() {
  uint8_t a = registers.getA();
  uint8_t value = memory.read(registers.PC + 1);
  uint8_t carry = registers.isFlagSet(registers.FLAG_C) ? 1 : 0;
  uint16_t result = registers.getA() - value - carry;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, true);
  registers.setFlag(registers.FLAG_H, isHalfCarrySubWithCarry(a, value, carry));
  registers.setFlag(registers.FLAG_C, isCarrySubWithCarry(a, value, carry));

  registers.setA(result & 0xff);
}

void CPU::AndAA() {
  uint8_t a = registers.getA();
  registers.setA(a & a);

  registers.setFlag(registers.FLAG_Z, registers.getA() == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, true);
  registers.setFlag(registers.FLAG_C, false);
}

void CPU::SubAB() {
  uint8_t a = registers.getA();
  uint8_t b = registers.getB();
  uint8_t result = a - b;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, true);
  registers.setFlag(registers.FLAG_H, isHalfCarrySub(a, b));
  registers.setFlag(registers.FLAG_C, isCarrySub(a, b));

  registers.setA(result);
}

void CPU::SubAC() {
  uint8_t a = registers.getA();
  uint8_t c = registers.getC();
  uint8_t result = a - c;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, true);
  registers.setFlag(registers.FLAG_H, isHalfCarrySub(a, c));
  registers.setFlag(registers.FLAG_C, isCarrySub(a, c));

  registers.setA(result);
}

void CPU::AndAC() {
  registers.setA(registers.getA() & registers.getC());

  registers.setFlag(registers.FLAG_Z, registers.getA() == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, true);
  registers.setFlag(registers.FLAG_C, false);
}

void CPU::RST08h() {
  uint16_t pc = registers.PC + 1;

  registers.SP -= 1;
  memory.write(registers.SP, pc & 0xff);

  registers.SP -= 1;
  memory.write(registers.SP, (pc >> 8) & 0xff);

  registers.PC = 0x08;
  jumped = true;
}

void CPU::PopDE() { registers.DE = popFromStack(); }

void CPU::LDHLu16() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  registers.HL = toLittleEndian(lowByte, highByte);
}

void CPU::LDHLPlusA() {
  uint8_t a = registers.getA();
  memory.write(registers.HL, a);
  registers.HL++;
}

void CPU::IncHL() { registers.HL++; }

void CPU::AddHLHL() {

  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H,
                    isHalfCarryAdd16(registers.HL, registers.HL));
  registers.setFlag(registers.FLAG_C, isCarryAdd16(registers.HL, registers.HL));

  registers.HL = registers.HL + registers.HL;
}

void CPU::LDAHLInc() {
  auto value = memory.read(registers.HL);
  registers.setA(value);
  registers.HL = registers.HL + 1;
}

void CPU::DecHL() { registers.HL--; }

void CPU::IncL() { registers.setL(registers.getL() + 1); }

void CPU::LDLu8() {
  uint8_t value = memory.read(registers.PC + 1);
  registers.setL(value);
}

void CPU::CPL() {
  registers.setA(~registers.getA());
  registers.setFlag(registers.FLAG_N, true);
  registers.setFlag(registers.FLAG_H, true);
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
  registers.HL--;
}

void CPU::IncHLAddr() {
  uint8_t value = memory.read(registers.HL);
  uint8_t result = value + 1;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, isHalfCarryAdd(value, 1));

  memory.write(registers.HL, result);
}

void CPU::DecAddrHL() {
  uint8_t value = memory.read(registers.HL);
  uint8_t result = value - 1;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, true);
  registers.setFlag(registers.FLAG_H, isHalfCarrySub(value, 1));

  memory.write(registers.HL, result);
}

void CPU::LDHLu8() {
  uint8_t value = memory.read(registers.PC + 1);
  memory.write(registers.HL, value);
}

void CPU::JRCi8() {
  int8_t offset = static_cast<int8_t>(memory.read(registers.PC + 1));

  if (registers.isFlagSet(registers.FLAG_C)) {
    registers.PC = registers.PC + opcodes[opcode].length + offset;
    jumped = true;
  }
}

void CPU::LDAHLDec() {
  registers.setA(memory.read(registers.HL));
  registers.HL--;
}

void CPU::DecSP() { registers.SP--; }

void CPU::IncA() { registers.setA(registers.getA() + 1); }

void CPU::DecA() {

  uint8_t a = registers.getA();
  uint8_t result = a - 1;

  registers.setFlag(registers.FLAG_H, isHalfCarrySub(a, 1));

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, true);

  registers.setA(result);
}

void CPU::ANDAL() {
  registers.setA(registers.getA() & registers.getL());

  registers.setFlag(registers.FLAG_Z, registers.getA() == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, true);
  registers.setFlag(registers.FLAG_C, false);
}

void CPU::XORAC() {
  uint8_t a = registers.getA();
  uint8_t c = registers.getC();
  uint8_t result = a ^ c;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, false);
  registers.setFlag(registers.FLAG_C, false);

  registers.setA(result);
}

void CPU::LDAu8() { registers.setA(memory.read(registers.PC + 1)); }

void CPU::LDBD() { registers.setB(registers.getD()); }

void CPU::LDCA() {
  uint8_t a = registers.getA();
  registers.setC(a);
}

void CPU::LDDHL() {
  uint8_t value = memory.read(registers.HL);
  registers.setD(value);
}

void CPU::LDEA() {
  uint8_t a = registers.getA();
  registers.setE(a);
}

void CPU::LDHA() { registers.setH(registers.getA()); }

void CPU::LDLE() { registers.setL(registers.getE()); }

void CPU::LDLA() { registers.setL(registers.getA()); }

void CPU::LDDA() {
  uint8_t a = registers.getA();
  registers.setD(a);
}

void CPU::LDEC() { registers.setE(registers.getC()); }

void CPU::LDEHL() {
  uint8_t value = memory.read(registers.HL);
  registers.setE(value);
}

void CPU::LDBA() {
  uint8_t a = registers.getA();
  registers.setB(a);
}

void CPU::LDFF00Plusu8A() {
  auto a = registers.getA();
  auto byte = memory.read(registers.PC + 1);
  uint16_t address = 0xff00 + byte;
  memory.write(address, a);
}

void CPU::PopHL() { registers.HL = popFromStack(); }

void CPU::LDFF00PlusCA() {
  uint8_t a = registers.getA();
  uint8_t c = registers.getC();
  uint16_t address = 0xff00 + c;

  memory.write(address, a);
}

void CPU::PushHL() { pushToStack(registers.HL); }

void CPU::AndAu8() {
  uint8_t a = registers.getA();
  uint8_t value = memory.read(registers.PC + 1);
  uint8_t result = a & value;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, true);
  registers.setFlag(registers.FLAG_C, false);
}

void CPU::AddSPi8() {
  int8_t offset = static_cast<int8_t>(memory.read(registers.PC + 1));

  registers.setFlag(registers.FLAG_Z, false);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H,
                    isHalfCarryAdd(registers.SP & 0xff, offset));
  registers.setFlag(registers.FLAG_C, isCarryAdd(registers.SP & 0xff, offset));

  registers.SP += offset;
}

void CPU::LDu16A() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  uint16_t address = toLittleEndian(lowByte, highByte);

  uint8_t value = registers.getA();
  memory.write(address, value);
}

void CPU::XORAu8() {
  uint8_t a = registers.getA();
  uint8_t value = memory.read(registers.PC + 1);
  uint8_t result = a ^ value;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, false);
  registers.setFlag(registers.FLAG_C, false);

  registers.setA(result);
}

void CPU::LDAFF00Plusu8() {
  auto byte = memory.read(registers.PC + 1);
  auto address = 0xff00 + byte;
  uint8_t value = memory.read(address);

  registers.setA(value);
}

void CPU::PopAF() { registers.AF = popFromStack(); }

void CPU::DI() { IME = 0; }

void CPU::PushAF() { pushToStack(registers.AF); }

void CPU::LDHLA() {
  uint8_t a = registers.getA();
  uint16_t address = registers.HL;
  memory.write(address, a);
}

void CPU::LDAB() { registers.setA(registers.getB()); }

void CPU::LDAD() { registers.setA(registers.getD()); }

void CPU::LDAu16() {
  auto [lowByte, highByte] = getLowByteAndHighByte();
  uint16_t address = toLittleEndian(lowByte, highByte);
  auto value = memory.read(address);
  registers.setA(value);
}

void CPU::LDAE() { registers.setA(registers.getE()); }

void CPU::LDAH() { registers.setA(registers.getH()); }

void CPU::LDAL() { registers.setA(registers.getL()); }

void CPU::LDAHL() {
  uint16_t address = registers.HL;
  uint8_t value = memory.read(address);
  registers.setA(value);
}

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

void CPU::AddAB() {
  uint8_t a = registers.getA();
  uint8_t b = registers.getB();
  uint16_t result = a + b;

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, isHalfCarryAdd(a, b));
  registers.setFlag(registers.FLAG_C, isCarryAdd(a, b));

  registers.setA(result);
}

void CPU::ADCAA() {
  uint8_t a = registers.getA();
  uint8_t oldCarry = registers.isFlagSet(registers.FLAG_C) ? 1 : 0;

  bool halfCarry = isHalfCarryAddWithCarry(a, a, oldCarry);
  bool carry = isCarryAddWithCarry(a, a, oldCarry);

  registers.setA(a + a + oldCarry);
  uint8_t newA = registers.getA();

  registers.setFlag(registers.FLAG_Z, newA == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, halfCarry);
  registers.setFlag(registers.FLAG_C, carry);
}

void CPU::ADCAC() {
  uint8_t a = registers.getA();
  uint8_t c = registers.getC();

  uint8_t oldCarry = registers.isFlagSet(registers.FLAG_C) ? 1 : 0;

  bool halfCarry = isHalfCarryAddWithCarry(a, c, oldCarry);
  bool carry = isCarryAddWithCarry(a, c, oldCarry);

  registers.setA(a + c + oldCarry);
  uint8_t newA = registers.getA();

  registers.setFlag(registers.FLAG_Z, newA == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, halfCarry);
  registers.setFlag(registers.FLAG_C, carry);
}

void CPU::EI() { IME = 1; }

// extended opcodes
void CPU::SwapA() {
  uint8_t a = registers.getA();
  uint8_t result = (a << 4) | (a >> 4);

  registers.setFlag(registers.FLAG_Z, result == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, false);
  registers.setFlag(registers.FLAG_C, false);

  registers.setA(result);
}

void CPU::Bit0A() {
  bool bit = ((registers.getA() & (1 << 0)) == 0);
  registers.setFlag(registers.FLAG_Z, bit == 0);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, true);
}

void CPU::Bit5A() {
  bool bit = ((registers.getA() & (1 << 5)) == 0);
  registers.setFlag(registers.FLAG_Z, bit);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, true);
}

void CPU::Bit6A() {
  bool bit = ((registers.getA() & (1 << 6)) == 0);
  registers.setFlag(registers.FLAG_Z, bit);
  registers.setFlag(registers.FLAG_N, false);
  registers.setFlag(registers.FLAG_H, true);
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

bool CPU::isHalfCarryAddWithCarry(uint8_t val1, uint8_t val2, uint8_t carry) {
  return isHalfCarryAdd(val1, val2) || isHalfCarryAdd(val1 + val2, carry);
}

bool CPU::isHalfCarrySub(uint8_t a, uint8_t value) {
  return (a & 0x0f) < (value & 0x0f);
}

bool CPU::isCarrySub(uint8_t a, uint8_t value) { return a < value; }

bool CPU::isCarryAdd(uint8_t a, uint8_t value) {
  return (uint16_t)a + (uint16_t)value > 0xFF;
}

bool CPU::isCarryAddWithCarry(uint8_t val1, uint8_t val2, uint8_t carry) {
  return ((uint16_t)val1 + (uint16_t)val2 + carry) > 0xFF;
}

bool CPU::isCarrySubWithCarry(uint8_t val1, uint8_t val2, uint8_t carry) {
  return (uint16_t)val1 < ((uint16_t)val2 + carry);
}

bool CPU::isHalfCarrySubWithCarry(uint8_t val1, uint8_t val2, uint8_t carry) {
  return ((val1 & 0x0F) < ((val2 & 0x0F) + carry));
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

uint16_t CPU::popFromStack() {
  uint8_t low = memory.read(registers.SP);
  uint8_t high = memory.read(registers.SP + 1);
  registers.SP += 2;

  return (high << 8) | low;
}

bool CPU::isHalfCarryAdd16(uint16_t a, uint16_t b) {
  return ((a & 0x0FFF) + (b & 0x0FFF)) > 0x0FFF;
}

bool CPU::isCarryAdd16(uint16_t a, uint16_t b) {
  return (uint32_t)a + (uint32_t)b > 0xFFFF;
}
