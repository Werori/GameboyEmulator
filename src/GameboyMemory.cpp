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
  // std::cout << "ADDRESS: " << address << std::endl;
  if (romBank0Range.contains(address)) {
    // std::cout << "Reading from bank0..." << std::endl;
    return romBank0[address - romBank0Range.start];
  } else if (romBank1Range.contains(address)) {
    std::cout << "Reading from bank1..." << std::endl;
    return romBank1[address - romBank1Range.start];
  } else if (vRAMRange.contains(address)) {
    return vram[address - vRAMRange.start];
  } else if (externalRAMRange.contains(address)) {
    std::cout << "Reading from external ram..." << std::endl;
    return externalRam[address - externalRAMRange.start];
  } else if (wRam1Range.contains(address)) {
    std::cout << "Reading from wram1..." << std::endl;
    return wram1[address - wRam1Range.start];
  } else if (wRam2Range.contains(address)) {
    std::cout << "Reading from wram2..." << std::endl;
    return wram2[address - wRam2Range.start];
  } else if (echoRamRange.contains(address)) {
    std::cout << "Reading echo ram" << std::endl;
    return wram1[(address - 0x2000) - wRam1Range.start];
  } else if (oamRange.contains(address)) {
    std::cout << "Reading from oam..." << std::endl;
    return oam[address - oamRange.start];
  } else if (ioRange.contains(address)) {
    return readIORegister(address);
  } else if (highRamRange.contains(address)) {
    std::cout << "Reading from highRam..." << std::endl;
    return highRam[address - highRamRange.start];
  } else if (address == 0xffff) {
    throw std::runtime_error("reading interrupt enable");
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
  case 0xff01:
    throw std::runtime_error("reading serial transfer data");
  case 0xff02:
    throw std::runtime_error("reading serial Transfer control");
  case 0xff04:
    throw std::runtime_error("reading divider register");
  case 0xff05:
    throw std::runtime_error("reading timer counter");
  case 0xff06:
    throw std::runtime_error("reading timer modulo");
  case 0xff07:
    throw std::runtime_error("reading timer control");
  case 0xff0f:
    return hardwareRegisters.interruptFlag;
  case 0xff10:
    throw std::runtime_error("reading sound channel 1 sweep");
  case 0xff11:
    throw std::runtime_error("reading sound channel 1 length timer");
  case 0xff12:
    throw std::runtime_error("reading sound channel 1 volume and envelope");
  case 0xff14:
    throw std::runtime_error("reading sound channel 1 period high and control");
  case 0xff16:
    throw std::runtime_error("reading sound channel 2 length timer");
  case 0xff17:
    throw std::runtime_error("reading sound channel 2 volume and envelope");
  case 0xff19:
    throw std::runtime_error("reading sound channel 2 period high and control");
  case 0xff1a:
    throw std::runtime_error("reading sound channel 3 DAC enable");
  case 0xff1c:
    throw std::runtime_error("reading sound channel 3 output level");
  case 0xff1e:
    throw std::runtime_error("reading sound channel 3 period high and control");
  case 0xff21:
    throw std::runtime_error("reading sound channel 4 volume and envelope");
  case 0xff22:
    throw std::runtime_error("reading sound channel 4 frequency and random");
  case 0xff23:
    throw std::runtime_error("reading sound channel 4 control");
  case 0xff24:
    throw std::runtime_error("reading master volume and vin panning");
  case 0xff25:
    throw std::runtime_error("reading sound panning");
  case 0xff26:
    throw std::runtime_error("reading sound on off");
  case 0xff40:
    return hardwareRegisters.lcdControl;
    // throw std::runtime_error("reading lcd control");
  case 0xff41:
    throw std::runtime_error("reading lcd status");
  case 0xff42:
    throw std::runtime_error("reading viewport y position");
  case 0xff43:
    throw std::runtime_error("reading viewport x position");
  case 0xff44:
    throw std::runtime_error("reading ly");
  case 0xff45:
    throw std::runtime_error("reading ly compare");
  case 0xff46:
    throw std::runtime_error("reading oam dma");
  case 0xff47:
    throw std::runtime_error("reading bg pallete data");
  case 0xff48:
    throw std::runtime_error("reading obj pallete 0");
  case 0xff49:
    throw std::runtime_error("reading obj pallete 1");
  case 0xff4a:
    throw std::runtime_error("reading window y position");
  case 0xff4b:
    throw std::runtime_error("reading window x + 7");
  }
  return 0;
}

void GameboyMemory::write(uint16_t address, uint8_t byte) {
  if (romBank0Range.contains(address)) {
    std::cout << "Can't write to ROM!" << std::endl;
  } else if (romBank1Range.contains(address)) {
    std::cout << "Can't write to ROM!" << std::endl;
  } else if (vRAMRange.contains(address)) {
    std::cout << "Writing to vram..." << std::endl;
    throw std::runtime_error("writing to vram");
    vram[address - vRAMRange.start] = byte;
  } else if (externalRAMRange.contains(address)) {
    std::cout << "Writing to external ram..." << std::endl;
    externalRam[address - externalRAMRange.start] = byte;
  } else if (wRam1Range.contains(address)) {
    std::cout << "Writing to wram1..." << std::endl;
    wram1[address - wRam1Range.start] = byte;
  } else if (wRam2Range.contains(address)) {
    std::cout << "Writing to wram2..." << std::endl;
    wram2[address - wRam2Range.start] = byte;
  } else if (echoRamRange.contains(address)) {
    throw std::runtime_error("writing echo ram");
  } else if (oamRange.contains(address)) {
    std::cout << "Writing to oam..." << std::endl;
    oam[address - oamRange.start] = byte;
  } else if (ioRange.contains(address)) {
    writeIORegister(address, byte);
  } else if (highRamRange.contains(address)) {
    std::cout << "Writing to highRam..." << std::endl;
    highRam[address - highRamRange.start] = byte;
  } else if (address == 0xffff) {
    interruptEnable = byte;
  } else {
    throw std::runtime_error("out of range address");
  }
}

void GameboyMemory::writeIORegister(uint16_t address, uint8_t byte) {
  if (address >= 0xff30 && address <= 0xff3f) {
    throw std::runtime_error("writing storage");
  }

  switch (address) {
  case 0xff00:
    throw std::runtime_error("writing joypad");
  case 0xff01:
    // needed for multiplayer
  case 0xff02:
    // needed for multiplayer
  case 0xff04:
    hardwareRegisters.dividerRegister = 0;
  case 0xff05:
    throw std::runtime_error("writing timer counter");
  case 0xff06:
    throw std::runtime_error("writing timer modulo");
  case 0xff07:
    throw std::runtime_error("writing timer control");
  case 0xff0f:
    hardwareRegisters.interruptFlag = byte;
  case 0xff10:
    hardwareRegisters.soundChannel1Sweep = byte;
    // throw std::runtime_error("writing sound channel 1 sweep");
  case 0xff11:
    hardwareRegisters.soundChannel1LengthTimer = byte;
    // std::cout << "writing sound channel 1 length timer" << std::endl;
    // throw std::runtime_error("writing sound channel 1 length timer");
  case 0xff12:
    hardwareRegisters.soundChannel1VolumeAndEnvelope = byte;
    // std::cout << "writing sound channel 1 volume and envelope" << std::endl;
    // ;
    // throw std::runtime_error("writing sound channel 1 volume and envelope");
  case 0xff13:
    hardwareRegisters.soundChannel1PeriodLow = byte;
    // std::cout << "writing sound channel 1 period low" << std::endl;
    // throw std::runtime_error("writing sound channel 1 period low");
  case 0xff14:
    hardwareRegisters.soundChannel1PeriodLow = byte;
    // std::cout << "writing sound channel 1 period high and control" <<
    // std::endl; throw std::runtime_error("writing sound channel 1 period high
    // and control");
  case 0xff16:
    hardwareRegisters.soundChannel2LengthTimer = byte;
    // std::cout << "writing sound channel 2 length timer" << std::endl;
    // throw std::runtime_error("writing sound channel 2 length timer");
  case 0xff17:
    hardwareRegisters.soundChannel2VolumeAndEnvelope = byte;
    // std::cout << "writing sound channel 2 volume and envelope" << std::endl;
    // throw std::runtime_error("writing sound channel 2 volume and envelope");
  case 0xff18:
    hardwareRegisters.soundChannel2PeriodLow = byte;
    // std::cout << "writing sound channel 2 period low" << std::endl;
    // throw std::runtime_error("writing sound channel 2 period low");
  case 0xff19:
    hardwareRegisters.soundChannel2PeriodHighAndControl = byte;
    // std::cout << "writing sound channel 2 period high and control" <<
    // std::endl; throw std::runtime_error("writing sound channel 2 period high
    // and control");
  case 0xff1a:
    hardwareRegisters.soundChannel3DACEnable = byte;
    // std::cout << "writing sound channel 3 DAC enable" << std::endl;
    // throw std::runtime_error("writing sound channel 3 DAC enable");
  case 0xff1b:
    hardwareRegisters.soundChannel3LengthTimer = byte;
    // std::cout << "writing sound channel 3 length timer" << std::endl;
    // throw std::runtime_error("writing sound channel 3 length timer");
  case 0xff1c:
    hardwareRegisters.soundChannel3OutputLevel = byte;
    // std::cout << "writing sound channel 3 output level" << std::endl;
    // throw std::runtime_error("writing sound channel 3 output level");
  case 0xff1d:
    hardwareRegisters.soundChannel3PeriodLow = byte;
    // std::cout << "writing sound channel 3 period low" << std::endl;
    // throw std::runtime_error("writing sound channel 3 period low");
  case 0xff1e:
    hardwareRegisters.soundChannel3PeriodHighAndControl = byte;
    // std::cout << "writing sound channel 3 period high and control" <<
    // std::endl; throw std::runtime_error("writing sound channel 3 period high
    // and control");
  case 0xff20:
    hardwareRegisters.soundChannel4LengthTimer = byte;
    // std::cout << "writing sound channel 4 length timer" << std::endl;
    // throw std::runtime_error("writing sound channel 4 length timer");
  case 0xff21:
    hardwareRegisters.soundChannel4VolumeAndEnvelope = byte;
    // std::cout << "writing sound channel 4 volume and envelope" << std::endl;
    // throw std::runtime_error("writing sound channel 4 volume and envelope");
  case 0xff22:
    hardwareRegisters.soundChannel4FrequencyAndRandom = byte;
    // std::cout << "writing sound channel 4 frequency and random" << std::endl;
    // throw std::runtime_error("writing sound channel 4 frequency and random");
  case 0xff23:
    hardwareRegisters.soundChannel4Control = byte;
    // std::cout << "writing sound channel 4 control" << std::endl;
    // throw std::runtime_error("writing sound channel 4 control");
  case 0xff24:
    hardwareRegisters.masterVolumeAndVinPanning = byte;
    // std::cout << "writing master volume and vin panning" << std::endl;
    // throw std::runtime_error("writing master volume and vin panning");
  case 0xff25:
    hardwareRegisters.soundPanning = byte;
    // std::cout << "writing sound panning" << std::endl;
    // throw std::runtime_error("writing sound panning");
  case 0xff26:
    hardwareRegisters.soundOnOff = byte;
    // throw std::runtime_error("writing sound on off");
  case 0xff40:
    hardwareRegisters.lcdControl = byte;
    // throw std::runtime_error("writing lcd control");
  case 0xff41:
    hardwareRegisters.lcdStatus = byte;
    // throw std::runtime_error("writing lcd status");
  case 0xff42:
    hardwareRegisters.viewportYPosition = byte;
    // throw std::runtime_error("writing viewport y position");
  case 0xff43:
    hardwareRegisters.viewportXPosition = byte;
    // throw std::runtime_error("writing viewport x position");
  case 0xff45:
    hardwareRegisters.lyc = byte;
    // throw std::runtime_error("writing ly compare");
  case 0xff46:
    startOamDmaTransfer(byte);
    // throw std::runtime_error("writing oam dma");
  case 0xff47:
    hardwareRegisters.bgPallete = byte;
    // throw std::runtime_error("writing bg pallete data");
  case 0xff48:
    hardwareRegisters.objPallete0 = byte;
  case 0xff49:
    hardwareRegisters.objPallete1 = byte;
  case 0xff4a:
    hardwareRegisters.windowY = byte;
  case 0xff4b:
    hardwareRegisters.windowXPlus7 = byte;
    // case 0xff50:
    // throw std::runtime_error("boot rom disable!");
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
