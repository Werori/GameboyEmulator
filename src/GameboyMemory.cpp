#include "GameboyMemory.h"
#include "InterruptFlag.h"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

GameboyMemory::GameboyMemory() {
  std::fill(romBank0.begin(), romBank0.end(), 0);
  std::fill(romBank1.begin(), romBank1.end(), 0);
  std::fill(vram.begin(), vram.end(), 0);
  std::fill(wram1.begin(), wram1.end(), 0);
  std::fill(wram2.begin(), wram2.end(), 0);
  std::fill(oam.begin(), oam.end(), 0);
  std::fill(ioRegisters.begin(), ioRegisters.end(), 0);
}

void GameboyMemory::loadROM(const std::vector<uint8_t> &romData) {
  constexpr size_t BANK_1_START = 2 * ROM_BANK_SIZE;

  std::copy(romData.begin(), romData.begin() + ROM_BANK_SIZE, romBank0.begin());

  if (romData.size() >= BANK_1_START) {
    std::copy(romData.begin() + ROM_BANK_SIZE, romData.begin() + BANK_1_START,
              romBank1.begin());
  }
}

uint8_t GameboyMemory::read(uint16_t address) {
  // std::cout << "ADDRESS READ: " << address << std::endl;
  if (romBank0Range.contains(address)) {
    // std::cout << "Reading from bank0..." << std::endl;
    return romBank0[address - romBank0Range.start];
  } else if (romBank1Range.contains(address)) {
    // std::cout << "Reading from bank1..." << std::endl;
    return romBank1[address - romBank1Range.start];
  } else if (vRAMRange.contains(address)) {
    // throw std::runtime_error("reading vram");
    return vram[address - vRAMRange.start];
  } else if (externalRAMRange.contains(address)) {
    throw std::runtime_error("reading external RAM");
    // std::cout << "Reading from external ram..." << std::endl;
    return externalRam[address - externalRAMRange.start];
  } else if (wRam1Range.contains(address)) {
    // throw std::runtime_error("reading wRam1");
    // std::cout << "Reading from wram1..." << std::endl;
    return wram1[address - wRam1Range.start];
  } else if (wRam2Range.contains(address)) {
    // throw std::runtime_error("reading wRam2");
    // std::cout << "Reading from wram2..." << std::endl;
    return wram2[address - wRam2Range.start];
  } else if (echoRamRange.contains(address)) {
    throw std::runtime_error("reading echo ");
    // std::cout << "Reading echo ram" << std::endl;
    return wram1[address - echoRamRange.start];
  } else if (oamRange.contains(address)) {
    // std::cout << "Reading from oam..." << std::endl;
    return oam[address - oamRange.start];
  } else if (ioRange.contains(address)) {
    return readIORegister(address);
  } else if (highRamRange.contains(address)) {
    // std::cout << "Reading from highRam..." << std::endl;
    return highRam[address - highRamRange.start];
  } else if (address == 0xffff) {
    return interruptEnable;
  } else {
    throw std::runtime_error("out of range address");
  }
}

uint8_t GameboyMemory::readIORegister(uint16_t address) {
  if (address >= 0xff30 && address <= 0xff3f) {
    throw std::runtime_error("reading storage");
  }

  switch (address) {
  case 0xff00:
    throw std::runtime_error("reading joypad");
    break;
  case 0xff01:
    throw std::runtime_error("reading serial transfer data");
    break;
  case 0xff02:
    throw std::runtime_error("reading serial Transfer control");
    break;
  case 0xff04:
    return hardwareRegisters.dividerRegister;
    // throw std::runtime_error("reading divider register");
    break;
  case 0xff05:
    throw std::runtime_error("reading timer counter");
    break;
  case 0xff06:
    throw std::runtime_error("reading timer modulo");
    break;
  case 0xff07:
    throw std::runtime_error("reading timer control");
    break;
  case 0xff0f:
    // throw std::runtime_error("reatind interrupt flag");
    return hardwareRegisters.interruptFlag;
    break;
  case 0xff10:
    throw std::runtime_error("reading sound channel 1 sweep");
    break;
  case 0xff11:
    throw std::runtime_error("reading sound channel 1 length timer");
    break;
  case 0xff12:
    throw std::runtime_error("reading sound channel 1 volume and envelope");
    break;
  case 0xff14:
    throw std::runtime_error("reading sound channel 1 period high and control");
    break;
  case 0xff16:
    throw std::runtime_error("reading sound channel 2 length timer");
    break;
  case 0xff17:
    throw std::runtime_error("reading sound channel 2 volume and envelope");
    break;
  case 0xff19:
    throw std::runtime_error("reading sound channel 2 period high and control");
    break;
  case 0xff1a:
    throw std::runtime_error("reading sound channel 3 DAC enable");
    break;
  case 0xff1c:
    throw std::runtime_error("reading sound channel 3 output level");
    break;
  case 0xff1e:
    throw std::runtime_error("reading sound channel 3 period high and control");
    break;
  case 0xff21:
    throw std::runtime_error("reading sound channel 4 volume and envelope");
    break;
  case 0xff22:
    throw std::runtime_error("reading sound channel 4 frequency and random");
    break;
  case 0xff23:
    throw std::runtime_error("reading sound channel 4 control");
    break;
  case 0xff24:
    throw std::runtime_error("reading master volume and vin panning");
    break;
  case 0xff25:
    throw std::runtime_error("reading sound panning");
    break;
  case 0xff26:
    throw std::runtime_error("reading sound on off");
    break;
  case 0xff40:
    // throw std::runtime_error("reading lcd control");
    return hardwareRegisters.lcdControl;
    break;
  case 0xff41:
    // throw std::runtime_error("reading lcd status");
    return hardwareRegisters.lcdStatus;
    break;
  case 0xff42:
    throw std::runtime_error("reading scx");
    break;
  case 0xff43:
    throw std::runtime_error("reading scy");
    break;
  case 0xff44:
    // throw std::runtime_error("reading ly");
    return hardwareRegisters.ly;
  case 0xff45:
    // throw std::runtime_error("reading lyc");
    return hardwareRegisters.lyc;
    break;
  case 0xff46:
    throw std::runtime_error("reading oam dma");
    break;
  case 0xff47:
    throw std::runtime_error("reading bg pallete data");
    break;
  case 0xff48:
    throw std::runtime_error("reading obj pallete 0");
    break;
  case 0xff49:
    throw std::runtime_error("reading obj pallete 1");
    break;
  case 0xff4a:
    throw std::runtime_error("reading window y position");
    break;
  case 0xff4b:
    throw std::runtime_error("reading window x + 7");
    break;
  default:
    throw std::runtime_error("out of range io");
  }
  return 0;
}

void GameboyMemory::write(uint16_t address, uint8_t byte) {
  // std::cout << "ADDRESS WRITE: " << std::hex << (int)address << std::endl;
  if (romBank0Range.contains(address)) {
    // throw std::runtime_error("can't write to rom bank0");
  } else if (romBank1Range.contains(address)) {
    throw std::runtime_error("can't write to rom bank1");
  } else if (vRAMRange.contains(address)) {
    // std::cout << "Writing to vram..." << std::endl;
    // throw std::runtime_error("writing to vram");
    vram[address - vRAMRange.start] = byte;
  } else if (externalRAMRange.contains(address)) {
    // std::cout << "Writing to external ram..." << std::endl;
    externalRam[address - externalRAMRange.start] = byte;
  } else if (wRam1Range.contains(address)) {
    // throw std::runtime_error("writing wram1");
    // std::cout << "Writing to wram1..." << std::endl;
    wram1[address - wRam1Range.start] = byte;
  } else if (wRam2Range.contains(address)) {
    // throw std::runtime_error("writing wram2");
    // std::cout << "Writing to wram2..." << std::endl;
    wram2[address - wRam2Range.start] = byte;
  } else if (echoRamRange.contains(address)) {
    throw std::runtime_error("writing echo ram");
  } else if (oamRange.contains(address)) {
    // std::cout << "Writing to oam..." << std::endl;
    oam[address - oamRange.start] = byte;
  } else if (ioRange.contains(address)) {
    writeIORegister(address, byte);
  } else if (highRamRange.contains(address)) {
    // std::cout << "Writing to highRam..." << std::endl;
    highRam[address - highRamRange.start] = byte;
  } else if (address == 0xffff) {
    interruptEnable = byte;
  } else {
    // std::cout << "out of range address " << (int)address << std::endl;
    // throw std::runtime_error("out of range address");
  }
}

void GameboyMemory::writeIORegister(uint16_t address, uint8_t byte) {
  if (address >= 0xff30 && address <= 0xff3f) {
    throw std::runtime_error("writing storage");
  }

  switch (address) {
  case 0xff00:
    throw std::runtime_error("writing joypad");
    break;
  case 0xff01:
    // throw std::runtime_error("writing serial transfer data");
    hardwareRegisters.serialTransferData = byte;
    // needed for multiplayer
    break;
  case 0xff02:
    // throw std::runtime_error("writing serial transfer control");
    hardwareRegisters.serialTransferControl = byte;
    if (byte & 0x80) {
      // Simulate immediate transfer completion
      hardwareRegisters.serialTransferControl &= ~0x80; // clear the start bit
      requestInterrupt(InterruptFlag::Serial);
    }
    // needed for multiplayer
    break;
  case 0xff04:
    throw std::runtime_error("writing divider register");
    hardwareRegisters.dividerRegister = 0;
    break;
  case 0xff05:
    throw std::runtime_error("writing timer counter");
    break;
  case 0xff06:
    throw std::runtime_error("writing timer modulo");
    break;
  case 0xff07: {
    // throw std::runtime_error("writing timerControl");
    break;
  case 0xff0f:
    // throw std::runtime_error("writing interrupt flag");
    hardwareRegisters.interruptFlag = byte;
    break;
  case 0xff10:
    throw std::runtime_error("writing sound channel 1 sweep");
    hardwareRegisters.soundChannel1Sweep = byte;
    break;
    throw std::runtime_error("writing sound channel 1 sweep");
  case 0xff11:
    // hardwareRegisters.soundChannel1LengthTimer = byte;
    // std::cout << "writing sound channel 1 length timer" << std::endl;
    throw std::runtime_error("writing sound channel 1 length timer");
    break;
  case 0xff12:
    // hardwareRegisters.soundChannel1VolumeAndEnvelope = byte;
    // std::cout << "writing sound channel 1 volume and envelope" << std::endl;
    // ;
    throw std::runtime_error("writing sound channel 1 volume and envelope");
    break;
  case 0xff13:
    // hardwareRegisters.soundChannel1PeriodLow = byte;
    // std::cout << "writing sound channel 1 period low" << std::endl;
    throw std::runtime_error("writing sound channel 1 period low");
    break;
  case 0xff14:
    // hardwareRegisters.soundChannel1PeriodLow = byte;
    // std::cout << "writing sound channel 1 period high and control" <<
    throw std::runtime_error("writing sound channel 1 period high");
    break;
  case 0xff16:
    throw std::runtime_error("writing sound channel 2 length timer");
    break;
    // std::cout << "writing sound channel 2 length timer" << std::endl;
    hardwareRegisters.soundChannel2LengthTimer = byte;
  case 0xff17:
    throw std::runtime_error("writing sound channel 2 volume and envelope");
    hardwareRegisters.soundChannel2VolumeAndEnvelope = byte;
    break;
    // std::cout << "writing sound channel 2 volume and envelope" << std::endl;
  case 0xff18:
    throw std::runtime_error("writing sound channel 2 period low");
    hardwareRegisters.soundChannel2PeriodLow = byte;
    break;
    // std::cout << "writing sound channel 2 period low" << std::endl;
  case 0xff19:

    throw std::runtime_error("writing sound channel 2 period high");
    // and control");    hardwareRegisters.soundChannel2PeriodHighAndControl =
    // byte;
    break;
    // std::cout << "writing sound channel 2 period high and control" <<
    // std::endl;
  case 0xff1a:
    throw std::runtime_error("writing sound channel 3 DAC enable");
    hardwareRegisters.soundChannel3DACEnable = byte;
    break;
    // std::cout << "writing sound channel 3 DAC enable" << std::endl;
  case 0xff1b:
    throw std::runtime_error("writing sound channel 3 length timer");
    hardwareRegisters.soundChannel3LengthTimer = byte;
    break;
    // std::cout << "writing sound channel 3 length timer" << std::endl;
  case 0xff1c:
    throw std::runtime_error("writing sound channel 3 output level");
    hardwareRegisters.soundChannel3OutputLevel = byte;
    break;
    // std::cout << "writing sound channel 3 output level" << std::endl;
  case 0xff1d:
    throw std::runtime_error("writing sound channel 3 period low");
    hardwareRegisters.soundChannel3PeriodLow = byte;
    break;
    // std::cout << "writing sound channel 3 period low" << std::endl;
  case 0xff1e:
    throw std::runtime_error("writing sound channel 3 period high and co");
    hardwareRegisters.soundChannel3PeriodHighAndControl = byte;
    break;
    // std::cout << "writing sound channel 3 period high and control" <<
    // std::endl;
  case 0xff20:
    throw std::runtime_error("writing sound channel 4 length timer");
    hardwareRegisters.soundChannel4LengthTimer = byte;
    break;
    // std::cout << "writing sound channel 4 length timer" << std::endl;
  case 0xff21:
    throw std::runtime_error("writing sound channel 4 volume and envelope");
    hardwareRegisters.soundChannel4VolumeAndEnvelope = byte;
    break;
    // std::cout << "writing sound channel 4 volume and envelope" << std::endl;
  case 0xff22:
    throw std::runtime_error("writing sound channel 4 frequency and random");
    hardwareRegisters.soundChannel4FrequencyAndRandom = byte;
    break;
    // std::cout << "writing sound channel 4 frequency and random" << std::endl;
  case 0xff23:
    throw std::runtime_error("writing sound channel 4 control");
    hardwareRegisters.soundChannel4Control = byte;
    break;
    // std::cout << "writing sound channel 4 control" << std::endl;
  case 0xff24:
    // throw std::runtime_error("writing master volume and vin panning");
    hardwareRegisters.masterVolumeAndVinPanning = byte;
    break;
    // std::cout << "writing master volume and vin panning" << std::endl;
  case 0xff25:
    // throw std::runtime_error("writing sound panning");
    hardwareRegisters.soundPanning = byte;
    break;
    // std::cout << "writing sound panning" << std::endl;
  case 0xff26:
    // throw std::runtime_error("writing sound on off");
    hardwareRegisters.soundOnOff = byte;
    break;
  case 0xff40:
    // throw std::runtime_error("writing lcd control");
    hardwareRegisters.lcdControl = byte;
    break;
  case 0xff41: {
    // throw std::runtime_error("writing lcd status");
    uint8_t currentStat = hardwareRegisters.lcdStatus;
    hardwareRegisters.lcdStatus = (byte & 0x78) | (currentStat & 0x87);
    break;
  }
  case 0xff42:
    // throw std::runtime_error("writing viewport y position");
    hardwareRegisters.scy = byte;
    break;
  case 0xff43:
    // throw std::runtime_error("writing viewport x position");
    hardwareRegisters.scx = byte;
    break;
  case 0xff45:
    // throw std::runtime_error("writing ly compare");
    hardwareRegisters.lyc = byte;
    break;
  case 0xff46:
    throw std::runtime_error("writing oam dma");
    startOamDmaTransfer(byte);
    break;
  case 0xff47:
    // throw std::runtime_error("writing bg pallete data");
    hardwareRegisters.bgPallete = byte;
    break;
  case 0xff48:
    // throw std::runtime_error("writing objpallete0");
    hardwareRegisters.objPallete0 = byte;
    break;
  case 0xff49:
    // throw std::runtime_error("writing objpallete1");
    hardwareRegisters.objPallete1 = byte;
    break;
  case 0xff4a:
    // throw std::runtime_error("writing wY");
    // std::cout << "wy: " << (int)byte << std::endl;
    hardwareRegisters.windowY = byte;
    break;
  case 0xff4b:
    throw std::runtime_error("writing wX");
    hardwareRegisters.windowX = byte;
    break;
  case 0xff50:
    throw std::runtime_error("boot rom disable!");
    // std::cout << "out of range io write" << std::endl;
    // throw std::runtime_error("out of range io write");
  }
    for (auto *observer : observers) {
      observer->onRegisterWrite(address, byte);
    }
  }
}
void GameboyMemory::requestInterrupt(InterruptFlag flag) {
  uint8_t interrupt_flag = read(0xff0f);
  interrupt_flag |= (1 << static_cast<uint8_t>(flag));
  write(0xFF0F, interrupt_flag);
}

void GameboyMemory::startOamDmaTransfer(uint8_t byte) {
  // source address is stored divided by 0x100 = 256 = address >> 8
  // so to get it back we have to multiply by 0x100 so << 8.
  uint16_t sourceAddress = byte << 8;

  // copy 160 bytes from source to OAM
  for (int i = 0; i < bytesToCopyIntoOAM; i++) {
    oam[i] = read(sourceAddress + i);
  }
}

void GameboyMemory::increaseDivider() { hardwareRegisters.dividerRegister++; }

void GameboyMemory::setLy(uint8_t value) { hardwareRegisters.ly = value; }

uint8_t GameboyMemory::getLy() { return hardwareRegisters.ly; }

uint8_t GameboyMemory::getScx() { return hardwareRegisters.scx; }

uint8_t GameboyMemory::getScy() { return hardwareRegisters.scy; }

uint8_t GameboyMemory::getWindowX() { return hardwareRegisters.windowX; }

uint8_t GameboyMemory::getWindowY() { return hardwareRegisters.windowY; }

void GameboyMemory::addObserver(IRegisterObserver *observer) {
  observers.push_back(observer);
}
