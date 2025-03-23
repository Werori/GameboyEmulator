#pragma once

#include <cstdint>

struct Range {
  uint16_t start;
  uint16_t end;

  Range(uint16_t start, uint16_t end);

  bool contains(uint16_t address);
};
