#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

struct CPU {
  uint16_t pc;
  uint16_t sp;

  static std::array<std::function<void()>, 256> opcodes;
  static std::array<size_t, 256> numOfBytes;

  CPU();

  static void initializeOpcodes();
  static void nop();
};
