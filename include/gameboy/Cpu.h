#pragma once

#include "GameboyMemory.h"
#include "Instruction.h"
#include "Registers.h"
#include <array>
#include <cstddef>
#include <cstdint>

struct CPU {
  GameboyMemory &memory;
  uint16_t opcode;
  std::array<Instruction, 256> opcodes;
  Registers registers;
  bool jumped = false;

  CPU(GameboyMemory &memory);

  void initializeOpcodes();
  void fetchOpcode();
  void decodeAndExecute();

  // this instruction does nothing
  static void nop();
  // Load 16-bit address into register BC
  void LDBCn16();
  // Jump to 16-bit address
  void JPa16();
};
