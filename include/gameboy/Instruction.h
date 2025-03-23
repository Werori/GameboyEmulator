#pragma once

#include <cstdint>
#include <functional>

struct Instruction {
  std::function<void()> func;
  uint8_t length;
  uint8_t machine_cycles_no_branch;
  uint8_t machine_cycles_branch;
};
