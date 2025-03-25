#pragma once

#include "GameboyMemory.h"
#include "Instruction.h"
#include "Registers.h"
#include <array>
#include <cstddef>
#include <cstdint>

struct CPU {
  GameboyMemory &memory;
  uint8_t opcode;
  std::array<Instruction, 256> opcodes;
  std::array<Instruction, 256> extendedOpcodes;
  Registers registers;
  bool jumped = false;

  CPU(GameboyMemory &memory);

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

  // switch high byte and low byte
  uint16_t toLittleEndian(uint8_t highByte, uint8_t lowByte);

  std::pair<uint8_t, uint8_t> getHighByteAndLowByte();
};
