#pragma once

#include "GameboyMemory.h"
#include "Instruction.h"
#include "Ppu.h"
#include "Registers.h"
#include "Timers.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>

struct CPU {
  GameboyMemory &memory;
  PPU &ppu;
  Timers &timers;
  uint8_t opcode;
  std::array<Instruction, 256> opcodes;
  std::array<Instruction, 256> extendedOpcodes;
  uint8_t IME = 1;
  bool IME_scheduled = false;

  bool halted = false;
  Registers registers;
  bool jumped = false;
  int cycles = 0;
  int length = 0;
  int lastInstructionCycles = 0;

  const uint8_t LOWER_NIBBLE_MASK = 0x0f;
  std::ofstream logfile;

  CPU(GameboyMemory &memory, PPU &ppu, Timers &timers);

  void initializeOpcodes();
  void initializeExtendedOpcodes();
  void fetchOpcode();
  void decodeAndExecute();

  // this instruction does nothing
  static void nop();
  // load 16-bit into BC
  void LDBCu16();
  // load A into address at BC
  void LDBCA();
  // increase BC
  void IncBC();
  // increase B
  void IncB();
  // decrease register B | set N flag, H&Z dependent
  void DecB();
  // load 8-bit into register B
  void LDBu8();
  // rotates bit left
  void RLCA();
  // load SP into u16
  void LDu16SP();
  // Add HL + BC, set flags
  void AddHLBC();
  // load value at address BC into A
  void LDABC();
  // decrease BC
  void DecBC();
  // increase C and set flags
  void IncC();
  // decrease register C | set N flag, H&Z dependent
  void DecC();
  // load 8-bit into register C
  void LDCu8();
  void RRCA();
  // stop and wait for input
  void STOP();
  // load 16-bit address into register DE
  void LDDEu16();
  // Jump to pc + offset
  void JRi8();
  // add DE to HL, set flags
  void AddHLDE();
  // load value at DE into A
  void LDADE();
  // decrease DE
  void DecDE();
  // Load A to address at DE
  void LDDEA();
  // increase DE
  void IncDE();
  // increase D
  void IncD();
  // decrease D
  void DecD();
  // set u8 to D
  void LDDu8();
  // increase E
  void IncE();
  // decrease E
  void DecE();
  // load u8 into E
  void LDEu8();
  // rotate A right, only carry flag
  void RRAn();
  // relative jump to pc + i8 if N flag is not set
  void JRNZi8();
  // load 16-bit address into register HL
  void LDHLu16();
  // load HL into A and increase HL
  void LDHLPlusA();
  // increase HL
  void IncHL();
  // increase H
  void IncH();
  // jump if not zero
  void JRZi8();
  // decrease H
  void DecH();
  // load u8 into H
  void LDHu8();
  // convert to BCD
  void DAA();
  // add HL to HL, set flags
  void AddHLHL();
  // load address at HL++ into A
  void LDAHLInc();
  // decrease HL
  void DecHL();
  // increase L
  void IncL();
  // decrease L
  void DecL();
  // load u8 into L
  void LDLu8();
  // negate A, set flags
  void CPL();
  // jump if not carry
  void JRNCi8();
  // load 16-bit valu into SP
  void LDSPu16();
  // load 8-bit address into register HL
  void LDHLu8();
  // jump if carry set
  void JRCi8();
  // Add HL + SP
  void AddHLSP();
  // load from address at HL into A, decrease HL
  void LDAHLDec();
  // decrease stack pointer
  void DecSP();
  // increase A
  void IncA();
  // decrease A
  void DecA();
  // get value from A and load it into address held by HL. Decrease HL.
  void LDHLDecA();
  // increase SP
  void IncSP();
  // increase value at HL and write to address HL
  void IncHLAddr();
  // decrease value at HL
  void DecAddrHL();
  // load u8 value into A register
  void LDAu8();
  // load B into B
  void LDBB();
  // load C into B
  void LDBC();
  // load D into B
  void LDBD();
  // load E into B
  void LDBE();
  // load H into B
  void LDBH();
  // load L into B
  void LDBL();
  // load value in address HL into B
  void LDBHL();
  // load value from A into C
  void LDCA();
  // load B into D
  void LDDB();
  // load C into D
  void LDDC();
  // load D into D
  void LDDD();
  // load E into D
  void LDDE();
  // load H into D
  void LDDH();
  // load L into D
  void LDDL();
  // load B into E
  void LDEB();
  // load D into E
  void LDED();
  // load value at HL into D
  void LDDHL();
  // load A into E
  void LDEA();
  // load D into H
  void LDHD();
  // load value at HL into H
  void LDHHL();
  // load A into H
  void LDHA();
  void LDLC();
  void LDHB();
  // load E into L
  void LDLE();
  // load value at HL into L
  void LDLHL();
  // load A into L
  void LDLA();
  // load B into address at HL
  void LDHLaddrB();
  // load C into address at HL
  void LDHLaddrC();
  // load D into address at HL
  void LDHLaddrD();
  // load E into value at HL
  void LDHLE();
  void HALT();
  // load A into D
  void LDDA();
  // load C into E
  void LDEC();
  // load value at address HL int o E
  void LDEHL();
  // load L into E
  void LDEL();
  // copy value of register A into B
  void LDBA();
  // load B into C
  void LDCB();
  // load C into C
  void LDCC();
  // load D into C
  void LDCD();
  // load E into C
  void LDCE();
  // load H into C
  void LDCH();
  // load L into C
  void LDCL();
  // load value at address HL into C
  void LDCHL();
  // Load 16-bit address into register BC
  void LDBCn16();
  // XOR A register with A register and set Z flag if result = 0
  void XORAA();
  // bitwise A | B, set flags
  void ORAB();
  // bitwise A | C, set flags
  void ORAC();
  // A | D, set flags
  void ORAD();
  // A | value at HL
  void ORAHLaddr();
  // A | A, set flags
  void ORAA();
  // compare A and B
  void CPAB();
  // compare A and C
  void CPAC();
  // compare A and D
  void CPAD();
  // compare A and E
  void CPAE();
  // return from stack if not zero
  void RetNz();
  // pop address into BC
  void PopBC();
  // jump to addr if not zero
  void JPNzu16();
  // Jump to 16-bit address
  void JPa16();
  // Call u16 if not Zero
  void CallNZu16();
  // push BC onto stack
  void PushBC();
  // Add A + u8, set flags
  void AddAu8();
  void RST00h();
  void RST10h();
  void RST18h();
  void RST20h();
  void RST30h();
  // jump to address from stack if Zero flag is set
  void RetZ();
  // return from stack
  void RET();
  // Jump to u16 if zero flag set
  void JPZu16();
  // push pc to stack if zero flag is set and jump to address
  void CallZu16();
  // push pc onto stack and set pc to given address
  void Call();
  // add witch carry A u8
  void ADCAu8();
  // pop from stack if not carry
  void RetNc();
  // push DE onto stack
  void PushDE();
  // subtract u8 from A
  void SubAu8();
  // return from stack if carry
  void RetC();
  // set pc to stack and enable IME
  void Reti();
  // subtract with carry
  void SBCAu8();
  // A & A, set flags
  void AndAA();
  // subtract B from A
  void SubAB();
  // subtract C from A
  void SubAC();
  // A and C
  void AndAC();
  // A and L
  void ANDAL();
  // xor AC, set flags
  void XORAC();
  void XORAE();
  // A ^ L
  void XORAL();
  // A ^ (HL)
  void XORAHL();
  // push PC onto stack and jump to 0x08
  void RST08h();
  // pop address into DE
  void PopDE();
  void JPNCu16();
  void JPCu16();
  void CallNCu16();
  void CallCu16();
  // load data to 0xFF00 Plus 8-bit data in the next byte
  void LDFF00Plusu8A();
  // pop from stack
  void PopHL();
  // load value of A into ff00 + C
  void LDFF00PlusCA();
  // push HL onto stack
  void PushHL();
  // A & u8, set flags
  void AndAu8();
  // add signed offset to SP, set flags
  void AddSPi8();
  // jump to address in HL
  void JPHL();
  // load value of A into 16-bit address
  void LDu16A();
  // A ^ u8
  void XORAu8();
  // push pc to stack and jump to 0x28
  void RST28h();
  // load value of 0xFF00 plus 8-bit into register A
  void LDAFF00Plusu8();
  // pop address into AF
  void PopAF();
  // disable interrupt
  void DI();
  void LDHLSPi8();
  // push AF onto stack
  void PushAF();
  // A | u8
  void OrAu8();
  // load a into address at HL
  void LDHLA();
  // load B into A
  void LDAB();
  // load C into A
  void LDAC();
  // load D into A
  void LDAD();
  // load value at address into A
  void LDAu16();
  // load E into A
  void LDAE();
  // load H into A
  void LDAH();
  // load L into A
  void LDAL();
  // load value at address HL into A
  void LDAHL();
  // copy HL value into stack pointer
  void LDSPHL();
  // subtract u8 from value in A, discard result, set flags
  void CPAu8();
  // loads A into A
  void LDAA();
  // add A and B
  void AddAB();
  void AddAL();
  // Add A and A
  void AddAA();
  // add with carry
  void ADCAA();
  // add C to A with carry
  void ADCAC();
  // enable interrupt
  void EI();
  // jump to 0x38
  void RST38h();
  // rotate bits in C one to the right

  // extended opcodes
  void RRC();
  void RRB();
  // rotate bits in C to the right
  void RRD();
  // rotate bits in E to the right
  void RRE();
  void SLAA();
  // swap bits in A
  void SwapA();
  // shift b >> 1, set carry
  void SrlB();
  // check bit 0 of A
  void Bit0A();
  void Bit3A();
  // check bit 5 of A
  void Bit5A();
  // check bit 6 of A
  void Bit6A();
  void Bit7A();
  void Bit7HL();
  // reset bit 0 of A
  void Res0A();
  void Res0HL();

  void Set7HL();

  void openLogFile(const std::string &filename);
  void closeLogFile();
  void logCPUState();

private:
  // switch high byte and low byte
  uint16_t toLittleEndian(uint8_t lowByte, uint8_t highByte);

  std::pair<uint8_t, uint8_t> getLowByteAndHighByte();

  bool isHalfCarryAdd(uint8_t a, uint8_t value);
  bool isHalfCarrySub(uint8_t a, uint8_t value);

  bool isHalfCarryAddWithCarry(uint8_t val1, uint8_t val2, uint8_t carry);
  bool isCarryAddWithCarry(uint8_t val1, uint8_t val2, uint8_t carry);

  bool isCarryAdd(uint8_t a, uint8_t value);
  bool isCarrySub(uint8_t a, uint8_t value);

  void handleInterrupts();

  void checkInterrupts();

  void pushToStack(uint16_t address);
  uint16_t popFromStack();

  bool isHalfCarryAdd16(uint16_t a, uint16_t b);
  bool isCarryAdd16(uint16_t a, uint16_t b);

  bool isCarrySubWithCarry(uint8_t val1, uint8_t val2, uint8_t carry);
  bool isHalfCarrySubWithCarry(uint8_t val1, uint8_t val2, uint8_t carry);
};
