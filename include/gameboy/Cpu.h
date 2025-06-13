#pragma once

#include "GameboyMemory.h"
#include "Instruction.h"
#include "Ppu.h"
#include "Registers.h"
#include "Timers.h"
#include <array>
#include <cstddef>
#include <cstdint>

struct CPU {
  GameboyMemory &memory;
  PPU &ppu;
  Timers &timers;
  uint8_t opcode;
  std::array<Instruction, 256> opcodes;
  std::array<Instruction, 256> extendedOpcodes;
  uint8_t IME = 1;

  Registers registers;
  bool jumped = false;
  int cycles = 0;
  int length = 0;
  int lastInstructionCycles = 0;

  const uint8_t LOWER_NIBBLE_MASK = 0x0f;

  CPU(GameboyMemory &memory, PPU &ppu, Timers &timers);

  void initializeOpcodes();
  void initializeExtendedOpcodes();
  void fetchOpcode();
  void decodeAndExecute();

  // this instruction does nothing
  static void nop();
  // load 16-bit into BC
  void LDBCu16();
  // load A into address at BC
  void LDBCA();
  // increase BC
  void IncBC();
  // increase B
  void IncB();
  // decrease register B | set N flag, H&Z dependent
  void DecB();
  // load 8-bit into register B
  void LDBu8();
  // rotates bit left
  void RLCA();
  // load value at address BC into A
  void LDABC();
  // decrease BC
  void DecBC();
  // increase C and set flags
  void IncC();
  // decrease register C | set N flag, H&Z dependent
  void DecC();
  // load 8-bit into register C
  void LDCu8();
  // stop and wait for input
  void STOP();
  // load 16-bit address into register DE
  void LDDEu16();
  // Jump to pc + offset
  void JRi8();
  // add DE to HL, set flags
  void AddHLDE();
  // load value at DE into A
  void LDADE();
  // Load A to address at DE
  void LDDEA();
  // increase DE
  void IncDE();
  // set u8 to D
  void LDDu8();
  // increase E
  void IncE();
  // load u8 into E
  void LDEu8();
  // relative jump to pc + i8 if N flag is not set
  void JRNZi8();
  // load 16-bit address into register HL
  void LDHLu16();
  // load HL into A and increase HL
  void LDHLPlusA();
  // increase HL
  void IncHL();
  // jump if not zero
  void JRZi8();
  // decrease H
  void DecH();
  // load u8 into H
  void LDHu8();
  // add HL to HL, set flags
  void AddHLHL();
  // load address at HL++ into A
  void LDAHLInc();
  // decrease HL
  void DecHL();
  // increase L
  void IncL();
  // load u8 into L
  void LDLu8();
  // load 16-bit valu into SP
  void LDSPu16();
  // load 8-bit address into register HL
  void LDHLu8();
  // jump if carry set
  void JRCi8();
  // load from address at HL into A, decrease HL
  void LDAHLDec();
  // decrease stack pointer
  void DecSP();
  // increase A
  void IncA();
  // decrease A
  void DecA();
  // get value from A and load it into address held by HL. Decrease HL.
  void LDHLDecA();
  // increase value at HL and write to address HL
  void IncHLAddr();
  // decrease value at HL
  void DecAddrHL();
  // load u8 value into A register
  void LDAu8();
  // load D into B
  void LDBD();
  // load value from A into C
  void LDCA();
  // load value at HL into D
  void LDDHL();
  // load A into E
  void LDEA();
  // load A into H
  void LDHA();
  // load E into L
  void LDLE();
  // load A into L
  void LDLA();
  // load A into D
  void LDDA();
  // load C into E
  void LDEC();
  // load value at address HL int o E
  void LDEHL();
  // copy value of register A into B
  void LDBA();
  // Load 16-bit address into register BC
  void LDBCn16();
  // XOR A register with A register and set Z flag if result = 0
  void XORAA();
  // bitwise A | B, set flags
  void ORAB();
  // bitwise A | C, set flags
  void ORAC();
  // A | D, set flags
  void ORAD();
  // compare A and C
  void CPAC();
  // return from stack if not zero
  void RetNz();
  // Jump to 16-bit address
  void JPa16();
  // push BC onto stack
  void PushBC();
  // jump to address from stack if Zero flag is set
  void RetZ();
  // return from stack
  void RET();
  // Jump to u16 if zero flag set
  void JPZu16();
  // push pc to stack if zero flag is set and jump to address
  void CallZu16();
  // push pc onto stack and set pc to given address
  void Call();
  // push DE onto stack
  void PushDE();
  // subtract u8 from A
  void SubAu8();
  // subtract with carry
  void SBCAu8();
  // A & A, set flags
  void AndAA();
  // subtract B from A
  void SubAB();
  // subtract C from A
  void SubAC();
  // A and L
  void ANDAL();
  // push PC onto stack and jump to 0x08
  void RST08h();
  // pop address into DE
  void PopDE();
  // load data to 0xFF00 Plus 8-bit data in the next byte
  void LDFF00Plusu8A();
  // pop from stack
  void PopHL();
  // load value of A into ff00 + C
  void LDFF00PlusCA();
  // push HL onto stack
  void PushHL();
  // A & u8, set flags
  void AndAu8();
  // add signed offset to SP, set flags
  void AddSPi8();
  // load value of A into 16-bit address
  void LDu16A();
  // A ^ u8
  void XORAu8();
  // load value of 0xFF00 plus 8-bit into register A
  void LDAFF00Plusu8();
  // disable interrupt
  void DI();
  // push AF onto stack
  void PushAF();
  // load a into address at HL
  void LDHLA();
  // load B into A
  void LDAB();
  // load D into A
  void LDAD();
  // load value at address into A
  void LDAu16();
  // load E into A
  void LDAE();
  // load H into A
  void LDAH();
  // load L into A
  void LDAL();
  // load value at address HL into A
  void LDAHL();
  // copy HL value into stack pointer
  void LDSPHL();
  // subtract u8 from value in A, discard result, set flags
  void CPAu8();
  // loads A into A
  void LDAA();
  // add A and B
  void AddAB();
  // add with carry
  void ADCAA();
  // add C to A with carry
  void ADCAC();
  // enable interrupt
  void EI();

  // extended opcodes
  void Bit0A();
  // check bit 5 of A
  void Bit5A();
  // check bit 6 of A
  void Bit6A();

private:
  // switch high byte and low byte
  uint16_t toLittleEndian(uint8_t lowByte, uint8_t highByte);

  std::pair<uint8_t, uint8_t> getLowByteAndHighByte();

  bool isHalfCarryAdd(uint8_t a, uint8_t value);
  bool isHalfCarrySub(uint8_t a, uint8_t value);

  bool isHalfCarryAddWithCarry(uint8_t val1, uint8_t val2, uint8_t carry);
  bool isCarryAddWithCarry(uint8_t val1, uint8_t val2, uint8_t carry);

  bool isCarryAdd(uint8_t a, uint8_t value);
  bool isCarrySub(uint8_t a, uint8_t value);

  void handleInterrupts();

  void checkInterrupts();

  void pushToStack(uint16_t address);
  uint16_t popFromStack();

  bool isHalfCarryAdd16(uint16_t a, uint16_t b);
  bool isCarryAdd16(uint16_t a, uint16_t b);

  bool isCarrySubWithCarry(uint8_t val1, uint8_t val2, uint8_t carry);
  bool isHalfCarrySubWithCarry(uint8_t val1, uint8_t val2, uint8_t carry);
};
