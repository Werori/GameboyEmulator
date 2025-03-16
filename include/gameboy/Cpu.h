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

  CPU(GameboyMemory &memory);

  void initializeOpcodes();
  void fetchOpcode();
  void decodeAndExecute();

  static void nop();
  static void LDBCn16();
};
