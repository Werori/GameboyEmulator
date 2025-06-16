#include "GameboyMemory.h"
#include "InterruptFlag.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
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
    return romBank0[address - romBank0Range.start];
  } else if (romBank1Range.contains(address)) {
    return romBank1[address - romBank1Range.start];
  } else if (vRAMRange.contains(address)) {
    return vram[address - vRAMRange.start];
  } else if (externalRAMRange.contains(address)) {
    // throw std::runtime_error("reading external RAM");
    return externalRam[address - externalRAMRange.start];
  } else if (wRam1Range.contains(address)) {
    return wram1[address - wRam1Range.start];
  } else if (wRam2Range.contains(address)) {
    return wram2[address - wRam2Range.start];
  } else if (echoRamRange.contains(address)) {
    return wram1[address - echoRamRange.start];
  } else if (oamRange.contains(address)) {
    return oam[address - oamRange.start];
  } else if (ioRange.contains(address)) {
    return readIORegister(address);
  } else if (highRamRange.contains(address)) {
    return highRam[address - highRamRange.start];
  } else if (address == 0xffff) {
    return interruptEnable;
  } else {
    throw std::runtime_error("out of range read");
  }
}

uint8_t GameboyMemory::readIORegister(uint16_t address) {
  if (address >= 0xff30 && address <= 0xff3f) {
    throw std::runtime_error("reading storage");
  }

  switch (address) {
  case 0xff00:
    break;
  case 0xff01:
    return hardwareRegisters.serialTransferData;
    break;
  case 0xff02:
    throw std::runtime_error("reading serial Transfer control");
    break;
  case 0xff04:
    return hardwareRegisters.dividerRegister;
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
    return hardwareRegisters.lcdControl;
    break;
  case 0xff41:
    return hardwareRegisters.lcdStatus;
    break;
  case 0xff42:
    throw std::runtime_error("reading scx");
    break;
  case 0xff43:
    throw std::runtime_error("reading scy");
    break;
  case 0xff44:
    // std::cout << "ly returned: " << std::hex << (int)hardwareRegisters.ly
    //           << std::endl;
    return hardwareRegisters.ly;
    return 0x90;
    break;
  case 0xff45:
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
  for (auto *observer : observers) {
    joypadSet = false;
    uint8_t value = observer->onRegisterRead(address);
    // value != defaultKey
    // std::cout << "value: " << std::hex << (int)value << std::endl;
    if (joypadSet) {
      return value;
    }
  }
  return 0;
}

void GameboyMemory::write(uint16_t address, uint8_t byte) {
  if (romBank0Range.contains(address)) {
  } else if (romBank1Range.contains(address)) {
  } else if (vRAMRange.contains(address)) {
    vram[address - vRAMRange.start] = byte;
  } else if (externalRAMRange.contains(address)) {
    externalRam[address - externalRAMRange.start] = byte;
  } else if (wRam1Range.contains(address)) {
    wram1[address - wRam1Range.start] = byte;
  } else if (wRam2Range.contains(address)) {
    wram2[address - wRam2Range.start] = byte;
  } else if (echoRamRange.contains(address)) {
    wram1[address - echoRamRange.start] = byte;
  } else if (oamRange.contains(address)) {
    oam[address - oamRange.start] = byte;
  } else if (ioRange.contains(address)) {
    writeIORegister(address, byte);
  } else if (highRamRange.contains(address)) {
    highRam[address - highRamRange.start] = byte;
  } else if (address == 0xffff) {
    interruptEnable = byte;
  } else {
  }
}

void GameboyMemory::writeIORegister(uint16_t address, uint8_t byte) {
  if (address >= 0xff30 && address <= 0xff3f) {
    // throw std::runtime_error("writing wave pattern");
  }

  switch (address) {
  case 0xff00:
    break;
  case 0xff01:
    hardwareRegisters.serialTransferData = byte;
    // needed for multiplayer
    break;
  case 0xff02:
    hardwareRegisters.serialTransferControl = byte;
    if (byte & 0x80) {
      // Simulate immediate transfer completion
      hardwareRegisters.serialTransferControl &= ~0x80; // clear the start bit
      requestInterrupt(InterruptFlag::Serial);
    }
    break;
  case 0xff04:
    // throw std::runtime_error("writing divider register");
    hardwareRegisters.dividerRegister = 0;
    break;
  case 0xff05:
    // throw std::runtime_error("writing timer counter");
    break;
  case 0xff06:
    // throw std::runtime_error("writing timer modulo");
    break;
  case 0xff07:
    // throw std::runtime_error("writing timerControl");
    break;
  case 0xff0f:
    hardwareRegisters.interruptFlag = byte;
    break;
  case 0xff10:
    // throw std::runtime_error("writing sound channel 1 sweep");
    hardwareRegisters.soundChannel1Sweep = byte;
    break;
  case 0xff11:
    hardwareRegisters.soundChannel1LengthTimer = byte;
    // std::cout << "writing sound channel 1 length timer" << std::endl;
    // throw std::runtime_error("writing sound channel 1 length timer");
    break;
  case 0xff12:
    hardwareRegisters.soundChannel1VolumeAndEnvelope = byte;
    // throw std::runtime_error("writing sound channel 1 volume and envelope");
    break;
  case 0xff13:
    hardwareRegisters.soundChannel1PeriodLow = byte;
    // throw std::runtime_error("writing sound channel 1 period low");
    break;
  case 0xff14:
    hardwareRegisters.soundChannel1PeriodHighAndControl = byte;
    // throw std::runtime_error("writing sound channel 1 period high");
    break;
  case 0xff16:
    hardwareRegisters.soundChannel2LengthTimer = byte;
    // throw std::runtime_error("writing sound channel 2 length timer");
    break;
    // std::cout << "writing sound channel 2 length timer" << std::endl;
  case 0xff17:
    // throw std::runtime_error("writing sound channel 2 volume and envelope");
    hardwareRegisters.soundChannel2VolumeAndEnvelope = byte;
    break;
    // std::cout << "writing sound channel 2 volume and envelope" << std::endl;
  case 0xff18:
    // throw std::runtime_error("writing sound channel 2 period low");
    hardwareRegisters.soundChannel2PeriodLow = byte;
    break;
  case 0xff19:
    // throw std::runtime_error("writing sound channel 2 period high");
    hardwareRegisters.soundChannel2PeriodHighAndControl = byte;
    break;
  case 0xff1a:
    // throw std::runtime_error("writing sound channel 3 DAC enable");
    hardwareRegisters.soundChannel3DACEnable = byte;
    break;
  case 0xff1b:
    // throw std::runtime_error("writing sound channel 3 length timer");
    hardwareRegisters.soundChannel3LengthTimer = byte;
    break;
  case 0xff1c:
    // throw std::runtime_error("writing sound channel 3 output level");
    hardwareRegisters.soundChannel3OutputLevel = byte;
    break;
  case 0xff1d:
    // throw std::runtime_error("writing sound channel 3 period low");
    hardwareRegisters.soundChannel3PeriodLow = byte;
    break;
  case 0xff1e:
    // throw std::runtime_error("writing sound channel 3 period high and co");
    hardwareRegisters.soundChannel3PeriodHighAndControl = byte;
    break;
  case 0xff20:
    // throw std::runtime_error("writing sound channel 4 length timer");
    hardwareRegisters.soundChannel4LengthTimer = byte;
    break;
  case 0xff21:
    // throw std::runtime_error("writing sound channel 4 volume and envelope");
    hardwareRegisters.soundChannel4VolumeAndEnvelope = byte;
    break;
  case 0xff22:
    // throw std::runtime_error("writing sound channel 4 frequency and random");
    hardwareRegisters.soundChannel4FrequencyAndRandom = byte;
    break;
  case 0xff23:
    // throw std::runtime_error("writing sound channel 4 control");
    hardwareRegisters.soundChannel4Control = byte;
    break;
  case 0xff24:
    // throw std::runtime_error("writing master volume and vin panning");
    hardwareRegisters.masterVolumeAndVinPanning = byte;
    break;
  case 0xff25:
    hardwareRegisters.soundPanning = byte;
    break;
  case 0xff26:
    hardwareRegisters.soundOnOff = byte;
    break;
  case 0xff40:
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
    // throw std::runtime_error("writing wX");
    hardwareRegisters.windowX = byte;
    break;
  case 0xff50:
    throw std::runtime_error("boot rom disable!");
    // throw std::runtime_error("out of range io write");
  }
  for (auto *observer : observers) {
    observer->onRegisterWrite(address, byte);
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
