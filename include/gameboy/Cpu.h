#pragma once

#include "GameboyMemory.h"
#include "Instruction.h"
#include "Ppu.h"
#include "Registers.h"
#include <array>
#include <cstddef>
#include <cstdint>

struct CPU {
  GameboyMemory &memory;
  PPU &ppu;
  uint8_t opcode;
  std::array<Instruction, 256> opcodes;
  std::array<Instruction, 256> extendedOpcodes;
  uint8_t IME = 0;
  Registers registers;
  bool jumped = false;
  int cycles = 0;
  int length = 0;

  const uint8_t LOWER_NIBBLE_MASK = 0x0f;

  CPU(GameboyMemory &memory, PPU &ppu);

  void initializeOpcodes();
  void fetchOpcode();
  void decodeAndExecute();

  // this instruction does nothing
  static void nop();
  // decrease register B | set N flag, H&Z dependent
  void DecB();
  // load 8-bit into register B
  void LDBu8();
  // decrease register C | set N flag, H&Z dependent
  void DecC();
  // load 8-bit into register C
  void LDCu8();
  // relative jump to pc + i8 if N flag is not set
  void JRNZi8();
  // load 16-bit address into register HL
  void LDHLu16();
  // get value from A and load it into address held by HL. Decrease HL.
  void LDHLDecA();
  // load u8 value into A register
  void LDAu8();
  // Load 16-bit address into register BC
  void LDBCn16();
  // XOR A register with A register and set Z flag if result = 0
  void XORAA();
  // Jump to 16-bit address
  void JPa16();
  // load data to 0xFF00 Plus 8-bit data in the next byte
  void LDFF00Plusu8A();
  // load value of 0xFF00 plus 8-bit into register A
  void LDAFF00Plusu8();
  // disable interrupt
  void DI();
  // copy HL value into stack pointer
  void LDSPHL();
  // subtract u8 from value in A, discard result, set flags
  void CPAu8();

private:
  // switch high byte and low byte
  uint16_t toLittleEndian(uint8_t lowByte, uint8_t highByte);

  std::pair<uint8_t, uint8_t> getLowByteAndHighByte();

  bool isHalfCarry(uint8_t a, uint8_t value);

  bool isCarry(uint8_t a, uint8_t value);

  void handleInterrupts();

  void checkInterrupts();
};
