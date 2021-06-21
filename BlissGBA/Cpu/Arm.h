#pragma once
#include "../Utils.h"

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

//FIQ disable (0 = enable, 1 = disable)
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



enum class State : u8 {
	ARM = 0,
	THUMB
};

class Arm {
private:
	State state;
	/*
		in THUMB mode only R0-R7 (Lo registers) may be accessed freely,
		while R8-R12 and up (Hi registers) can be accessed 
		only by some instructions.
	*/
	u32 registers[0xC];

	u32 SP;
	//stores return addr when calling subroutine or branch instr
	u32 LR; 
	u32 PC;
	u32 CPSR;
};