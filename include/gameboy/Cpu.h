#pragma once

#include "GameboyMemory.h"
#include "Instruction.h"
#include <array>
#include <cstddef>
#include <cstdint>

struct CPU {
  uint16_t pc;
  uint16_t sp;
  GameboyMemory &memory;
  uint16_t opcode;

  std::array<Instruction, 256> opcodes;

  CPU(GameboyMemory &memory);

  void initializeOpcodes();
  void fetchOpcode();
  void decodeAndExecute();

  static void nop();
  static void LDBCn16();
};
