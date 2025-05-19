#pragma once

#include "InterruptFlag.h"

class InterruptController {
public:
  virtual void requestInterrupt(InterruptFlag flag) = 0;
  virtual ~InterruptController() = default;
};
