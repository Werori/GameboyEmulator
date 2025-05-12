#include "GameboyMemory.h"

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
  if (romBank0Range.contains(address)) {
    std::cout << "Reading from bank0..." << std::endl;
    return romBank0[address - romBank0Range.start];
  } else if (romBank1Range.contains(address)) {
    std::cout << "Reading from bank1..." << std::endl;
    return romBank1[address - romBank1Range.start];
  } else if (vRAMRange.contains(address)) {
    std::cout << "Reading from vram..." << std::endl;
    throw std::runtime_error("Reading from VRAM");
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
  } else if (oamRange.contains(address)) {
    std::cout << "Reading from oam..." << std::endl;
    return oam[address - oamRange.start];
  } else if (address == 0xff00) {
    throw std::runtime_error("reading joypad");
  } else if (address == 0xff01) {
    throw std::runtime_error("reading serial transfer data");
  } else if (address == 0xff02) {
    throw std::runtime_error("reading serial Transfer control");
  } else if (address == 0xff04) {
    throw std::runtime_error("reading divider register");
  } else if (address == 0xff05) {
    throw std::runtime_error("reading timer counter");
  } else if (address == 0xff06) {
    throw std::runtime_error("reading timer modulo");
  } else if (address == 0xff07) {
    throw std::runtime_error("reading timer control");
  } else if (address == 0xff0f) {
    throw std::runtime_error("reading interrupt flag");
  } else if (address == 0xff10) {
    throw std::runtime_error("reading sound channel 1 sweep");
  } else if (address == 0xff11) {
    throw std::runtime_error("reading sound channel 1 length timer");
  } else if (address == 0xff12) {
    throw std::runtime_error("reading sound channel 1 volume and envelope");
  } else if (address == 0xff14) {
    throw std::runtime_error("reading sound channel 1 period high and control");
  } else if (ioRange.contains(address)) {
    throw std::runtime_error("io range");
  } else if (highRamRange.contains(address)) {
    std::cout << "Reading from highRam..." << std::endl;
    return highRam[address - highRamRange.start];
  } else {
    throw std::runtime_error("out of range address");
  }
}
void GameboyMemory::write(uint16_t address, uint8_t byte) {
  if (romBank0Range.contains(address)) {
    std::cout << "Can't write to ROM!" << std::endl;
  } else if (romBank1Range.contains(address)) {
    std::cout << "Can't write to ROM!" << std::endl;
  } else if (vRAMRange.contains(address)) {
    std::cout << "Writing to vram..." << std::endl;
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
  } else if (oamRange.contains(address)) {
    std::cout << "Writing to oam..." << std::endl;
    oam[address - oamRange.start] = byte;
  } else if (address == 0xcf) {
    throw std::runtime_error("writing joypad");
  } else if (address == 0xff01) {
    throw std::runtime_error("writing serial transfer data");
  } else if (address == 0xff02) {
    throw std::runtime_error("writing serial transfer control");
  } else if (address == 0xff04) {
    throw std::runtime_error("writing divider register");
  } else if (address == 0xff05) {
    throw std::runtime_error("writing timer counter");
  } else if (address == 0xff06) {
    throw std::runtime_error("writing timer modulo");
  } else if (address == 0xff07) {
    throw std::runtime_error("writing timer control");
  } else if (address == 0xff0f) {
    throw std::runtime_error("writing interrupt flag");
  } else if (address == 0xff10) {
    throw std::runtime_error("writing sound channel 1 sweep");
  } else if (address == 0xff11) {
    throw std::runtime_error("writing sound channel 1 length timer");
  } else if (address == 0xff12) {
    throw std::runtime_error("writing sound channel 1 volume and envelope");
  } else if (address == 0xff13) {
    throw std::runtime_error("writing sound channel 1 period low");
  } else if (address == 0xff14) {
    throw std::runtime_error("writing sound channel 1 period high and control");
  } else if (ioRange.contains(address)) {
    throw std::runtime_error("io range");
  } else if (highRamRange.contains(address)) {
    std::cout << "Writing to highRam..." << std::endl;
    highRam[address - highRamRange.start] = byte;
  } else {
    throw std::runtime_error("out of range address");
  }
}
