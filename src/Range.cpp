#include "Range.h"
#include <cstdint>

Range::Range(uint16_t start, uint16_t end) : start(start), end(end) {}

bool Range::contains(uint16_t address) {
  return address >= start && address <= end;
}
