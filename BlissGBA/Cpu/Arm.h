#pragma once
#include "Instruction.h"
#include "AddressingModes.h"
#include <array>
#include <functional>

using namespace std::placeholders;
#define b(x) std::bind(x, this, _1)

/*
	The ARM7TDMI is a 32bit RISC (Reduced Instruction Set Computer) CPU, 
	designed by ARM (Advanced RISC Machines)
*/

//Current Program Status Register (CPSR) bits
//Mode bits M0 - M4
#define M0 (1 << 0)
#define M0_BIT 0
#define M1 (1 << 1)
#define M1_BIT 1
#define M2 (1 << 2)
#define M2_BIT 2
#define M3 (1 << 3)
#define M3_BIT 3
#define M4 (1 << 4)
#define M4_BIT 4

//State bit (0 = ARM, 1 = THUMB) 
//Do not change manually
#define T (1 << 5)
#define T_BIT 5

//FIQ disable (Fast interrupt request) (0 = enable, 1 = disable)
#define F (1 << 6)
#define F_BIT 6

//IRQ disable (0 = enable, 1 = disable)
#define I (1 << 7)
#define I_BIT 7

//Overflow flag (0 = no overflow, 1 = overflow)
#define V (1 << 28)
#define V_BIT 28

//Carry flag (0 = Borrow/No Carry, 1 = Carry/No Borrow)
#define C (1 << 29)
#define C_BIT 29

//Zero flag (0 = Not Zero, 1 = Zero)
#define Z (1 << 30)
#define Z_BIT 30

//Sign flag (0 = Not signed, 1 = Signed)
#define N (1 << 31)
#define N_BIT 31

//Control bits mask (0 - 7)
#define CONTROL_BITS (M0 | M1 | M2 | M3 | M4 | T | F | I)

//Flags field bit mask
#define FLAG_FIELD_BITS (V | C | Z | N)

//Program counter register format (R15)

#define NUM_REGISTERS 0xD
#define R13_ID 0xD
#define R14_ID 0xE
#define R15_ID 0xF

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

struct PSR {
	u32 CPSR;
	u32 SPSR;
};

class MemoryBus;

class Arm {
public:
	Arm(MemoryBus *mbus);
	u8 clock();
	void reset();
	void setFlag(u32 flagBits, bool condition);
	void setFlag(u32 flagBits);
	void clearFlag(u32 flagBits);
	void setFlagSPSR(u32 flagBits);
	void clearFlagSPSR(u32 flagBits);

	void fillPipeline();
	void flushPipeline();
	void flushThumbPipeline();

	u8 getFlag(u32 flag);
	u8 carryFrom(u32 op1, u32 op2);
	u8 borrowFrom(u32 op1, u32 op2);
	u8 overflowFromAdd(u32 op1, u32 op2);
	u8 overflowFromSub(u32 op1, u32 op2);
	u8 fetchOp(u32 encoding);
	PSR getPSR();

	u32 getRegister(RegisterID reg);
	void writeRegister(RegisterID reg, u32 value);
	void writePC(u32 pc);

	//Set condition codes (S bit)
	void setCC(u32 rd, bool borrow, bool overflow,
		 bool shiftOut = false, u8 shifterCarryOut = 0);

	State getState();
	u8 getConditionCode(u8 cond);

	u16 readU16();
	u16 fetchU16();
	u32 readU32();
	u32 fetchU32();

	u32 shift(u32 value, u8 amount, u8 type, u8 &shiftedBit);
	u32 lsl(u32 value, u8 shift, u8& shiftedBit);
	u32 lsr(u32 value, u8 shift, u8& shiftedBit);
	u32 asr(u32 value, u8 shift, u8& shiftedBit);
	u32 ror(u32 value, u8 shift);
	u32 rrx(u32 value, u8 &shiftedBit);

	void executeArmIns(ArmInstruction& ins);
	void executeThumbIns(ThumbInstruction& ins);

	//Arm
	u8 executeDataProcessing(ArmInstruction& ins, bool flags, bool immediate);
	u8 executeDataProcessingImmFlags(ArmInstruction& ins);
	u8 executeDataProcessingImm(ArmInstruction& ins);
	u8 executeDataProcessingImmShiftFlags(ArmInstruction& ins);
	u8 executeDataProcessingImmShift(ArmInstruction& ins);
	u8 executeDataProcessingRegShiftFlags(ArmInstruction& ins);
	u8 executeDataProcessingRegShift(ArmInstruction& ins);
	u8 handleUndefinedIns(ArmInstruction& ins);

	u8 executeLoadStore(ArmInstruction& ins, AddrModeLoadStoreResult& result);
	u8 executeLoadStoreImm(ArmInstruction& ins);
	u8 executeLoadStoreShift(ArmInstruction& ins);

	u8 executeMiscLoadAndStore(ArmInstruction& ins, AddrModeLoadStoreResult &result);
	u8 executeMiscLoadStoreImm(ArmInstruction& ins);
	u8 executeMiscLoadStoreReg(ArmInstruction& ins);

	//Load/Store Multiple
	u8 executeLDM(ArmInstruction& ins);
	u8 executeSTM(ArmInstruction& ins);

	//Status Register
	u8 executeMSRImm(ArmInstruction& ins);
	u8 executeMSRReg(ArmInstruction& ins);

	//Thumb
	u8 executeThumbUnconditionalBranch(ThumbInstruction& ins);

private:
	void mapArmOpcodes();
	void mapThumbOpcodes();

	//Arm Instructions

	//Data processing
	u8 opMOV(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opADD(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opAND(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opEOR(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opSUB(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opRSB(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opADC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opSBC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opRSC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opTST(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opTEQ(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opCMP(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opCMN(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opORR(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opBIC(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);
	u8 opMVN(ArmInstruction& ins, RegisterID rd, RegisterID rn,
		bool flags, bool immediate);

	//Branches
	u8 opB(ArmInstruction& ins);
	u8 opBL(ArmInstruction& ins);
	u8 opBX(ArmInstruction& ins);
	u8 opSWI(ArmInstruction& ins);

	//Misc Load/Stores
	u8 opSTRH(ArmInstruction &ins, RegisterID rd, u32 address);
	u8 opLDRH(ArmInstruction& ins, RegisterID rd, u32 address);
	u8 opLDRSB(ArmInstruction& ins, RegisterID rd, u32 address);
	u8 opLDRSH(ArmInstruction& ins, RegisterID rd, u32 address);
	u8 opSWP(ArmInstruction& ins, RegisterID rd);

	//Load/Stores
	u8 opLDRB(ArmInstruction& ins, RegisterID rd, u32 address);
	u8 opLDR(ArmInstruction& ins, RegisterID rd, u32 address);
	u8 opSTRB(ArmInstruction& ins, RegisterID rd, u32 address);
	u8 opSTR(ArmInstruction& ins, RegisterID rd, u32 address);

	//Status Register access instructions
	u8 opMSR(ArmInstruction& ins, u32 value);



	//Thumb instructions

	u8 thumbOpBCond(ThumbInstruction& ins);
	u8 thumbOpB(ThumbInstruction& ins);
	u8 thumbOpBL(ThumbInstruction& ins);

public:
	State state;
	/*
		in THUMB mode only R0-R7 (Lo registers) may be accessed freely,
		while R8-R12 and up (Hi registers) can be accessed 
		only by some instructions.
	*/
	Register registers[NUM_REGISTERS]; //R0 - R12

	u32 SP; //R13
	//Stores return addr when calling subroutine or branch instr
	u32 LR; //R14
	u32 R15; //contains PC
	u32 CPSR;
	u32 SPSR;

	AddressingMode1 addrMode1;
	AddressingMode2 addrMode2;
	AddressingMode3 addrMode3;
	AddressingMode4 addrMode4;

	std::array<std::function<u8(ArmInstruction&)>, 4096> armlut;
	std::array<std::function<u8(ThumbInstruction&)>, 256> thumblut;

	u32 armpipeline[2];
	u16 thumbpipeline[2];

	u32 cycles = 0;
	u32 currentExecutingOpcode;
	MemoryBus* mbus;
};