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
  void fetchOpcode();
  void decodeAndExecute();

  // this instruction does nothing
  static void nop();
  // load 16-bit into BC
  void LDBCu16();
  // decrease register B | set N flag, H&Z dependent
  void DecB();
  // load 8-bit into register B
  void LDBu8();
  // decrease BC
  void DecBC();
  // increase C and set flags
  void IncC();
  // decrease register C | set N flag, H&Z dependent
  void DecC();
  // load 8-bit into register C
  void LDCu8();
  // load 16-bit address into register DE
  void LDDEu16();
  // Load A to address at DE
  void LDDEA();
  // increase E
  void IncE();
  // relative jump to pc + i8 if N flag is not set
  void JRNZi8();
  // load 16-bit address into register HL
  void LDHLu16();
  // load address at HL++ into A
  void LDAHLInc();
  // load 16-bit valu into SP
  void LDSPu16();
  // load 8-bit address into register HL
  void LDHLu8();
  // get value from A and load it into address held by HL. Decrease HL.
  void LDHLDecA();
  // load u8 value into A register
  void LDAu8();
  // copy value of register A into B
  void LDBA();
  // Load 16-bit address into register BC
  void LDBCn16();
  // XOR A register with A register and set Z flag if result = 0
  void XORAA();
  // bitwise A | C, set flags
  void ORAC();
  // return from stack if not zero
  void RetNz();
  // Jump to 16-bit address
  void JPa16();
  // return from stack
  void RET();
  // push pc onto stack and set pc to given address
  void Call();
  // push PC onto stack and jump to 0x08
  void RST08h();
  // load data to 0xFF00 Plus 8-bit data in the next byte
  void LDFF00Plusu8A();
  // load value of A into ff00 + C
  void LDFF00PlusCA();
  // load value of A into 16-bit address
  void LDu16A();
  // load value of 0xFF00 plus 8-bit into register A
  void LDAFF00Plusu8();
  // disable interrupt
  void DI();
  // load B into A
  void LDAB();
  // load E into A
  void LDAE();
  // copy HL value into stack pointer
  void LDSPHL();
  // subtract u8 from value in A, discard result, set flags
  void CPAu8();
  // loads A into A
  void LDAA();
  // enable interrupt
  void EI();

private:
  // switch high byte and low byte
  uint16_t toLittleEndian(uint8_t lowByte, uint8_t highByte);

  std::pair<uint8_t, uint8_t> getLowByteAndHighByte();

  bool isHalfCarryAdd(uint8_t a, uint8_t value);
  bool isHalfCarrySub(uint8_t a, uint8_t value);

  bool isCarryAdd(uint8_t a, uint8_t value);
  bool isCarrySub(uint8_t a, uint8_t value);

  void handleInterrupts();

  void checkInterrupts();

  void pushToStack(uint16_t address);
};
