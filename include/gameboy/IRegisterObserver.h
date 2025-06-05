#include <cstdint>
class IRegisterObserver {
public:
  virtual void onRegisterWrite(uint16_t address, uint8_t byte) = 0;
  virtual ~IRegisterObserver() = default;
};
