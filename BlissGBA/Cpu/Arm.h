#pragma once
#include "Instruction.h"
#include <array>
#include <functional>

#define b(x) std::bind(x, this, std::placeholders::_1)

/*
	The ARM7TDMI is a 32bit RISC (Reduced Instruction Set Computer) CPU, 
	designed by ARM (Advanced RISC Machines)
*/

//Current Program Status Register (CPSR) bits
//Mode bits M0 - M4
#define M0 (1 << 0)
#define M1 (1 << 1)
#define M2 (1 << 2)
#define M3 (1 << 3)
#define M4 (1 << 4)

//State bit (0 = ARM, 1 = THUMB) 
//Do not change manually
#define T (1 << 5)

//FIQ disable (Fast interrupt request) (0 = enable, 1 = disable)
#define F (1 << 6)

//IRQ disable (0 = enable, 1 = disable)
#define I (1 << 7)

//Overflow flag (0 = no overflow, 1 = overflow)
#define V (1 << 28)

//Carry flag (0 = Borrow/No Carry, 1 = Carry/No Borrow)
#define C (1 << 29)

//Zero flag (0 = Not Zero, 1 = Zero)
#define Z (1 << 30)

//Sign flag (0 = Not signed, 1 = Signed)
#define N (1 << 31)

//Program counter register format (R15)


union Register {
	struct {
		u16 lo;
		u16 hi;
	};
	u32 value;
};

enum class State : u8 {
	ARM = 0,
	THUMB
};

class MemoryBus;

class Arm {
public:
	Arm(MemoryBus *mbus);
	u8 clock();
	void decodeAndExecute(u8 opcode);
	void reset();
	void setFlag(u32 flagBits, bool condition);
	void setFlag(u32 flagBits);
	void clearFlag(u32 flagBits);

	u8 fetchOp(u32 encoding);

	//Returns 24 bit PC from R15 in 26 bit mode
	u32 getPC();

	u32 readU32();
	u32 fetchU32();

private:
	void mapArmOpcodes();
	void mapThumbOpcodes();

	//Arm Instructions
	u8 opMOV(ArmInstruction& ins);

private:
	State state;
	/*
		in THUMB mode only R0-R7 (Lo registers) may be accessed freely,
		while R8-R12 and up (Hi registers) can be accessed 
		only by some instructions.
	*/
	Register registers[0xD]; //R0 - R12

	u32 SP; //R13
	//Stores return addr when calling subroutine or branch instr
	u32 LR; //R14
	u32 PC; //R15
	u32 CPSR;
	u32 SPSR;

	std::array<std::function<u8(ArmInstruction&)>, 0xF> armlut;
	std::array<std::function<u8(ThumbInstruction&)>, 0xF> thumblut;

	u32 cycles;
	MemoryBus* mbus;
};